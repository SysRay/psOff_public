#pragma once

#include "utility/utility.h"

#include <cstdint>
#include <functional>
#include <string>

class ITrophies {
  CLASS_NO_COPY(ITrophies);
  CLASS_NO_MOVE(ITrophies);

  protected:
  ITrophies() = default;

  public:
  ~ITrophies() = default;

  enum class ErrCodes {
    SUCCESS = 0,        // No errors, we're fine
    INVALID_CONTEXT,    // Context is nullptr
    NO_ITROP,           // There are no trophyset info in lightweight file
    CONTINUE,           // Not an actual error code. For internal usage only!
    NO_KEY_SET,         // No root key installed
    INVALID_MAGIC,      // TRP file has invalid magic in its header
    INVALID_VERSION,    // TRP file version is not valid
    INVALID_ENTSIZE,    // TRP file has bigger entries
    INVALID_AES,        // TRP file contains unaligned AES blocks
    NOT_IMPLEMENTED,    // This feature is not implemented yet
    IO_FAIL,            // Failed to read TRP file
    NO_CALLBACKS,       // Parser called with no callbacks, it's pointless
    DECRYPT,            // TRP file decryption failed
    NO_TROPHIES,        // Failed to open TRP file or the said file does not contain any esfm file
    MAX_TROPHY_REACHED, // The game hit the hard limit of 128 trophies
  };

  struct trp_grp_cb {
    struct data_t {
      int32_t     id;
      std::string name;
      std::string detail;
    } data;

    bool                         cancelled;
    std::function<bool(data_t*)> func;
  };

  struct trp_ent_cb {
    struct data_t {
      int32_t     id;
      int32_t     group;
      bool        hidden;
      uint8_t     grade;
      std::string name;
      std::string detail;
    } data;

    bool                         cancelled;
    std::function<bool(data_t*)> func;
  };

  struct trp_png_cb {
    struct data_t {
      std::string pngname;
      void*       pngdata;
      size_t      pngsize;
    } data;

    bool                         cancelled;
    std::function<bool(data_t*)> func;
  };

  struct trp_inf_cb {
    struct data_t {
      std::string title_name;
      std::string title_detail;
      std::string trophyset_version;
      uint32_t    trophy_count;
      uint32_t    group_count;
    } data;

    bool                         cancelled;
    std::function<bool(data_t*)> func;
  };

  struct trp_context {
    bool lightweight;

    trp_ent_cb entry = {.data = {}, .cancelled = false, .func = nullptr};
    trp_grp_cb group = {.data = {}, .cancelled = false, .func = nullptr};
    trp_png_cb pngim = {.data = {}, .cancelled = false, .func = nullptr};
    trp_inf_cb itrop = {.data = {}, .cancelled = false, .func = nullptr};

    inline bool cancelled() { return entry.cancelled && group.cancelled && pngim.cancelled && itrop.cancelled; }
  };

  virtual ErrCodes    parseTRP(trp_context* context) = 0;
  virtual const char* getError(ErrCodes ec)          = 0;

  virtual bool createContext(int32_t userId, uint32_t label)                      = 0;
  virtual bool destroyContext(int32_t userId)                                     = 0;
  virtual bool getProgress(int32_t userId, uint32_t progress[4], uint32_t* count) = 0;
  virtual bool unlockTrophy(int32_t userId, int32_t trophyId)                     = 0;
  virtual bool resetUserInfo(int32_t userId)                                      = 0;
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif
__APICALL ITrophies& accessTrophies();
#undef __APICALL
