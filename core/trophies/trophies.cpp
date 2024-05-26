#define __APICALL_EXTERN
#include "trophies.h"
#undef __APICALL_EXTERN

#include "core/fileManager/fileManager.h"
#include "modules/libSceNpTrophy/types.h"
#include "modules_include/system_param.h"
#include "tools/config_emu/config_emu.h"

#include <boost/scoped_ptr.hpp>
#include <fstream>
#include <openssl/evp.h>
#include <pugixml.hpp>

#define TR_AES_BLOCK_SIZE 16
#undef min // We don't need it there

class Trophies: public ITrophies {
  enum class cbtype {
    UNKNOWN,
    TROPHY_UNLOCK,
  };

  struct callback {
    cbtype  type;
    vvpfunc func;
  };

  bool                  m_bKeySet                  = false;
  bool                  m_bIgnoreMissingLocale     = false;
  uint8_t               m_trkey[TR_AES_BLOCK_SIZE] = {};
  std::string           m_localizedTrophyFile;
  std::filesystem::path m_npidCachePath;
  std::vector<callback> m_callbacks = {};
  std::mutex            m_mutexParse;

  private:
  std::vector<usr_context> m_ctx {};

  struct trp_header {
    uint32_t magic; // should be 0xDCA24D00
    uint32_t version;
    uint64_t tfile_size; // size of trp file
    uint32_t entry_num;  // num of entries
    uint32_t entry_size; // size of entry
    uint32_t dev_flag;   // 1: dev
    uint8_t  digest[20]; // sha hash
    uint32_t key_index;
    char     padding[44];
  };

  struct trp_entry {
    char     name[32];
    uint64_t pos;
    uint64_t len;
    uint32_t flag;
    char     padding[12];
  };

  static bool caseequal(char a, char b) { return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b)); }

  static ParserErr XML_parse(const char* mem, trp_context* ctx) {
    pugi::xml_document doc;

    auto res = doc.load_buffer(mem, strlen(mem));

    if (res.status != pugi::xml_parse_status::status_ok) return ParserErr::INVALID_XML;

    if (auto tc = doc.child("trophyconf")) {
      if (!ctx->itrop.cancelled) {
        ctx->itrop.data.trophyset_version.assign(tc.child("trophyset-version").first_child().text().as_string("[nover]"));
        ctx->itrop.data.title_name.assign(tc.child("title-name").first_child().text().as_string("[unnamed]"));
        ctx->itrop.data.title_detail.assign(tc.child("title-detail").first_child().text().as_string("[unnamed]"));
        ctx->itrop.data.trophy_count = ctx->itrop.data.group_count = 0;
      }

      for (auto node = tc.child("trophy"); node; node = node.next_sibling("trophy")) {
        if (!ctx->entry.cancelled) {
          ctx->entry.data.id       = node.attribute("id").as_int(-1);
          ctx->entry.data.group    = node.attribute("gid").as_int(-1);
          ctx->entry.data.platinum = node.attribute("pid").as_int(-1);
          ctx->entry.data.hidden   = node.attribute("hidden").as_bool(false);
          ctx->entry.data.grade    = std::tolower(*(node.attribute("ttype").as_string()));

          if (!ctx->lightweight) {
            ctx->entry.data.name.assign(node.child("name").first_child().text().as_string("[unnamed]"));
            ctx->entry.data.detail.assign(node.child("detail").first_child().text().as_string("[unnamed]"));
          }

          ctx->entry.cancelled = ctx->entry.func(&ctx->entry.data);
        }

        if (!ctx->itrop.cancelled) ++ctx->itrop.data.trophy_count;
      }

      for (auto node = tc.child("group"); node; node = node.next_sibling("group")) {
        if (!ctx->group.cancelled) {
          ctx->group.data.id = node.attribute("id").as_int(-1);

          if (!ctx->lightweight) {
            ctx->group.data.name.assign(node.child("name").first_child().text().as_string("[unnamed]"));
            ctx->group.data.detail.assign(node.child("detail").first_child().text().as_string("[unnamed]"));
          }

          ctx->group.cancelled = ctx->group.func(&ctx->group.data);
        }

        if (!ctx->itrop.cancelled) ++ctx->itrop.data.group_count;
      }

      if (!ctx->itrop.cancelled) ctx->itrop.func(&ctx->itrop.data);

      ctx->entry.cancelled = true;
      ctx->group.cancelled = true;
      ctx->itrop.cancelled = true;
      return ParserErr::CONTINUE;
    }

    return ParserErr::NO_TROPHIES;
  }

  ParserErr TRP_readentry(const trp_entry& ent, trp_entry& dent, std::ifstream& trfile, trp_context* ctx, uint32_t label) {
    if (!ctx->pngim.cancelled) {
      static std::string_view ext(".png");
      std::string_view        name(ent.name);
      if (std::equal(ext.rbegin(), ext.rend(), name.rbegin(), caseequal)) { // Test trp file extension
        if (((ent.flag >> 24) & 0x03) == 0) {
          ctx->pngim.data.pngsize = ent.len;
          ctx->pngim.data.pngname.assign(ent.name);
          if ((ctx->pngim.data.pngdata = ::calloc(ent.len, 1)) == nullptr) { // Developer should free this memory manually
            return ParserErr::OUT_OF_MEMORY;
          }
          trfile.seekg(ent.pos);
          if (trfile.read((char*)ctx->pngim.data.pngdata, ent.len)) {
            ctx->pngim.cancelled = ctx->pngim.func(&ctx->pngim.data);
            return ParserErr::CONTINUE;
          }

          return ParserErr::IO_FAIL;
        } else {
          // Is this even possible?
          return ParserErr::NOT_IMPLEMENTED;
        }
      }
    }

    if (!ctx->group.cancelled || !ctx->entry.cancelled || !ctx->itrop.cancelled) {
      static std::string_view ext(".esfm");
      std::string_view        name(ent.name);
      if (!std::equal(ext.rbegin(), ext.rend(), name.rbegin(), caseequal)) return ParserErr::CONTINUE;
      if ((ent.len % 16) != 0) return ParserErr::INVALID_AES;
      if (ctx->lightweight) {
        static std::string_view lwfile("tropconf.esfm");
        if (!std::equal(lwfile.begin(), lwfile.end(), name.begin(), name.end(), caseequal)) return ParserErr::CONTINUE;
      } else if (!m_bIgnoreMissingLocale) {
        static std::string_view dfile("trop.esfm");
        // Trying to find localized trophy
        if (!std::equal(name.begin(), name.end(), m_localizedTrophyFile.begin(), m_localizedTrophyFile.end(), caseequal)) {
          // Save the default one to `dent` variable. It will be used if no localized trophy configuration file found
          if (std::equal(name.begin(), name.end(), dfile.begin(), dfile.end(), caseequal)) dent = ent;
          return ParserErr::CONTINUE;
        }
      }

      boost::scoped_ptr<char> mem(new char[ent.len]);
      trfile.seekg(ent.pos); // Seek to file position

      if (((ent.flag >> 24) & 0x03) == 0) {
        if (!trfile.read(mem.get(), ent.len)) return ParserErr::IO_FAIL;
      } else {
        static constexpr int32_t IV_SIZE           = TR_AES_BLOCK_SIZE;
        static constexpr int32_t ENC_SCE_SIGN_SIZE = TR_AES_BLOCK_SIZE * 3; // 384 encrypted bits is just enough to find interesting for us string

        uint8_t d_iv[TR_AES_BLOCK_SIZE];
        uint8_t kg_iv[TR_AES_BLOCK_SIZE];
        uint8_t enc_xmlh[ENC_SCE_SIGN_SIZE];
        ::memset(kg_iv, 0, TR_AES_BLOCK_SIZE);

        if (!trfile.read((char*)d_iv, TR_AES_BLOCK_SIZE)) return ParserErr::IO_FAIL;
        if (!trfile.read((char*)enc_xmlh, ENC_SCE_SIGN_SIZE)) return ParserErr::IO_FAIL;

        const auto trydecrypt = [this, &mem, &ent, d_iv, kg_iv, enc_xmlh, &trfile](uint32_t npid) -> bool {
          uint8_t outbuffer[512];
          uint8_t inbuffer[512];

          ::memset(outbuffer, 0, 512);
          ::memset(inbuffer, 0, 512);
          ::sprintf_s((char*)inbuffer, sizeof(inbuffer), "NPWR%05d_00", npid);

          int outlen;

          //  Key creation context
          {
            EVP_CIPHER_CTX* key_ctx = EVP_CIPHER_CTX_new();
            if (!EVP_EncryptInit(key_ctx, EVP_aes_128_cbc(), m_trkey, kg_iv)) {
              EVP_CIPHER_CTX_free(key_ctx);
              return false;
            }
            if (!EVP_EncryptUpdate(key_ctx, outbuffer, &outlen, inbuffer, TR_AES_BLOCK_SIZE)) {
              EVP_CIPHER_CTX_free(key_ctx);
              return false;
            }
            /**
             * Cipher finalizing is not really necessary there,
             * since we use only 16 bytes encrypted by the update function above
             */
            EVP_CIPHER_CTX_free(key_ctx);
          }
          //- Key creation context

          //  Data decipher context
          EVP_CIPHER_CTX* data_ctx = EVP_CIPHER_CTX_new();
          {
            if (!EVP_DecryptInit(data_ctx, EVP_aes_128_cbc(), outbuffer /* the buffer holds the decryption key now */, d_iv)) {
              EVP_CIPHER_CTX_free(data_ctx);
              return false;
            }

            EVP_CIPHER_CTX_set_padding(data_ctx, 0);

            if (!EVP_DecryptUpdate(data_ctx, outbuffer, &outlen, enc_xmlh, ENC_SCE_SIGN_SIZE)) {
              EVP_CIPHER_CTX_free(data_ctx);
              return false;
            }
            if (::_strnicmp((char*)outbuffer, "<!--Sce-Np-Trophy-Signature", 27) != 0) {
              EVP_CIPHER_CTX_free(data_ctx);
              return false;
            }

            // We found valid NPID, now we can continue our thing
            ::memcpy(mem.get(), outbuffer, outlen);
            char* mem_off = mem.get() + outlen;
            // Seeking to unread encrypted data position (skip Init Vector + Signature Comkment Part)
            trfile.seekg(ent.pos + TR_AES_BLOCK_SIZE + ENC_SCE_SIGN_SIZE);

            size_t copied = ENC_SCE_SIGN_SIZE;
            while (copied < ent.len) {
              size_t len = std::min(ent.len - copied, sizeof(inbuffer));
              // Reading the rest of AES data block by block
              if (!trfile.read((char*)inbuffer, len)) {
                EVP_CIPHER_CTX_free(data_ctx);
                return false;
              }
              if (!EVP_DecryptUpdate(data_ctx, (uint8_t*)mem_off, &outlen, inbuffer, len)) {
                EVP_CIPHER_CTX_free(data_ctx);
                return false;
              }

              mem_off += outlen;
              copied += len;
            }

            if (!EVP_DecryptFinal(data_ctx, (uint8_t*)mem_off, &outlen)) {
              EVP_CIPHER_CTX_free(data_ctx);
              return false;
            }

            EVP_CIPHER_CTX_free(data_ctx);
            /**
             * OpenSSL AES decrypt garbage data workaround.
             * TODO: Fix it actually... EVP_DecryptUpdate writes
             * garbage bytes to the buffer after XML file ends for
             * some reason. I don't figured out yet why.
             */
            auto p = std::string_view(mem.get()).find("</trophyconf>");
            if (p != std::string_view::npos) *(mem.get() + p + 13) = '\0';
            *(mem_off + outlen) = '\0'; // Finally
          }
          //- Data decipher context

          return true;
        }; // lambda: trydecrypt

        int  npid;
        bool success = false;

        auto npidcpath = m_npidCachePath / std::format(".{}", label);

        {
          // Trying to obtain cached NPID for this title and use it for trophies decrypting
          std::ifstream npid_f(npidcpath);

          if (npid_f.is_open()) {
            npid_f >> npid;
            success = trydecrypt(npid);
          }
        }

        if (!success) { // We failed to decrypt xml with saved NPID, now we search it
          for (uint32_t n = 0; n < 99999; n++) {
            trfile.seekg(ent.pos + TR_AES_BLOCK_SIZE);
            if ((success = trydecrypt(n)) == true) {
              npid = n;
              break;
            }
          }

          if (success) { // NPID found, saving it to cache file
            std::ofstream npid_f(npidcpath, std::ios::out);

            if (npid_f.is_open()) {
              npid_f << npid;
            }
          }
        }

        if (!success) return ParserErr::DECRYPT;
      }

      return XML_parse(mem.get(), ctx);
    } // group & trophy callbacks

    return ParserErr::CONTINUE;
  }

  void callUnlockCallback(trp_unlock_data* cbdata) {
    for (auto& cb: m_callbacks) {
      if (cb.type == cbtype::TROPHY_UNLOCK) cb.func(cbdata);
    }

    if (cbdata->image.pngdata) {
      ::free(cbdata->image.pngdata);
      cbdata->image.pngdata = nullptr;
      cbdata->image.pngsize = 0ull;
    }
  }

  int32_t _unlockTrophyEx(usr_context* ctx, int32_t trophyId, int32_t* platinumId) {
    if (!m_bKeySet) return -1;
    int32_t platId = getPlatinumIdFor(ctx, trophyId);
    if (platId == -2) return Err::NpTrophy::PLATINUM_CANNOT_UNLOCK;
    if (platId == -3) return Err::NpTrophy::BROKEN_DATA;
    if (platId == -4) return Err::NpTrophy::INVALID_CONTEXT;

    auto pngname = std::format("TROP{:3}.PNG", trophyId);

    int32_t ret         = Ok;
    bool    platinumMet = (platId >= 0);

    trp_unlock_data cbdata = {.userId = ctx->userId, .label = ctx->label};

    trp_context tctx = {
        .entry =
            {
                .func = [this, ctx, platId, &platinumMet, &ret, &cbdata, trophyId](trp_ent_cb::data_t* data) -> bool {
                  if (ret != 0) return true;

                  if (data->id == trophyId) {
                    cbdata.id    = trophyId;
                    cbdata.grade = data->grade;
                    cbdata.name.assign(data->name);
                    cbdata.descr.assign(data->detail);
                  } else if (platinumMet && data->grade != 'p' && data->platinum == platId) {
                    platinumMet = getUnlockTime(ctx, data->id) > 0;
                  }

                  if (data->grade == 'p' && data->id == platId) {
                    cbdata.pname.assign(data->name);
                    cbdata.descr.assign(data->detail);
                  }

                  return false;
                },
            },
        .pngim =
            {
                .func = [ret, &cbdata, pngname](trp_png_cb::data_t* data) -> bool {
                  if (data->pngname == pngname) {
                    cbdata.image.pngdata = data->pngdata;
                    cbdata.image.pngsize = data->pngsize;
                    return true;
                  }

                  return false;
                },
            },
    };

    ParserErr ec;
    if ((ec = parseTRP(ctx->label, &tctx)) != ParserErr::SUCCESS) {
      ret = Err::NpTrophy::BROKEN_DATA;
    }

    if (ret != Ok) {
      if (cbdata.image.pngdata) ::free(cbdata.image.pngdata);
      return ret;
    }

    if (cbdata.id != trophyId) return Err::NpTrophy::INVALID_TROPHY_ID;

    if (platinumMet) {
      cbdata.platGained = true;
      *platinumId = cbdata.platId = platId;
    } else {
      cbdata.platGained = false;
      cbdata.platId     = platId;
      *platinumId       = -1;
    }

    callUnlockCallback(&cbdata);

    return Ok;
  }

  void checkPlatinumTrophies(usr_context* ctx) {

    if (!m_bKeySet) return;

    struct platdata {
      int32_t     id;
      std::string name;
      std::string descr;
    };

    std::vector<platdata> unlocked_plats = {};

    trp_context tctx = {
        .entry =
            {
                .func = [this, ctx, &unlocked_plats](trp_ent_cb::data_t* data) -> bool {
                  // Marking all platinum trophies as unlocked to check if they actually unlocked later
                  if (data->grade == 'p' && getUnlockTime(ctx, data->id) == 0ull) {
                    unlocked_plats.push_back({data->id, data->name, data->detail});
                  }
                  return false;
                },
            },
    };

    if (parseTRP(ctx->label, &tctx) != ParserErr::SUCCESS) return;

    tctx.entry.func = [this, ctx, &unlocked_plats](trp_ent_cb::data_t* data) -> bool {
      if (data->grade != 'p' && data->platinum != -1) {
        // Some trophy is not unlocked yet, removing the platinum trophy from the unlocked list then
        if (getUnlockTime(ctx, data->id) == 0ull) {
          for (auto it = unlocked_plats.begin(); it != unlocked_plats.end();) {
            if (it->id == data->platinum) {
              unlocked_plats.erase(it);
              break;
            }
          }
        }
      }

      return false;
    };

    if (parseTRP(ctx->label, &tctx) != ParserErr::SUCCESS || unlocked_plats.size() == 0ull) return;
    uint64_t uTime = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::utc_clock::now().time_since_epoch()).count();

    for (auto& plat: unlocked_plats) {
      trp_unlock_data cbdata = {
          .grade = 'p',
          .name  = plat.name,
          .descr = plat.descr,
      };

      callUnlockCallback(&cbdata);
      ctx->trophies.push_back({plat.id, 0, uTime});
      saveTrophyData(ctx);
    }
  }

  public:
  Trophies() {
    auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::GENERAL);
    std::string hexdata;

    if (getJsonParam(jData, "trophyKey", hexdata) && hexdata.length() > 31) {
      if (hexdata.starts_with("0x")) hexdata.erase(0, 2);
      if (hexdata.find_first_not_of("0123456789abcdefABCDEF") != hexdata.npos) return;
      m_bKeySet = true;
      for (uint32_t i = 0; i < 32; i += 2) {
        auto byte      = hexdata.substr(i, 2);
        m_trkey[i / 2] = (uint8_t)std::strtol(byte.c_str(), nullptr, 16);
      }
    }

    {
      SystemParamLang systemlang;

      try {
        (*jData)["systemlang"].get_to(systemlang);
      } catch (const json::exception& e) {
        systemlang = SystemParamLang::EnglishUS;
      }

      m_localizedTrophyFile = std::format("TROP_{:02}.ESFM", (uint32_t)systemlang);
    }

    m_npidCachePath = accessFileManager().getGameFilesDir() / ".npcommid";
  }

  //  Callbacks

  void addTrophyUnlockCallback(vvpfunc func) final { m_callbacks.push_back({cbtype::TROPHY_UNLOCK, func}); }

  //- Callbacks

  ParserErr parseTRP(uint32_t label, trp_context* ctx) final {
    if (!m_bKeySet) return ParserErr::NO_KEY_SET;
    if (ctx == nullptr) return ParserErr::INVALID_CONTEXT;
    // We don't want every callback to be cancelled before we even begin
    ctx->group.cancelled = (ctx->group.func == nullptr);
    ctx->entry.cancelled = (ctx->entry.func == nullptr);
    ctx->pngim.cancelled = (ctx->pngim.func == nullptr);
    ctx->itrop.cancelled = (ctx->itrop.func == nullptr);
    if (ctx->cancelled()) return ParserErr::NO_CALLBACKS;

    std::unique_lock lock(m_mutexParse);
    m_bIgnoreMissingLocale = false;

    auto mpath = accessFileManager().getMappedPath(std::format("/app0/sce_sys/trophy/trophy{:02}.trp", label));
    if (!mpath.has_value()) return ParserErr::NO_TROPHIES;

    std::ifstream trfile(mpath->c_str(), std::ios::in | std::ios::binary);

    if (trfile.is_open()) {
      trp_header hdr;

      if (!trfile.read((char*)&hdr, sizeof(hdr))) return ParserErr::IO_FAIL;
      if (hdr.magic != 0x004DA2DC) return ParserErr::INVALID_MAGIC;
      if (hdr.version != 0x03000000) return ParserErr::INVALID_VERSION;

      /**
       * This file's numbers encoded in big-endian for some twisted reason.
       * Probably because of format history.
       */
      hdr.entry_num  = _byteswap_ulong(hdr.entry_num);
      hdr.entry_size = _byteswap_ulong(hdr.entry_size);
      if (hdr.entry_size != sizeof(trp_entry)) return ParserErr::INVALID_ENTSIZE;

      trp_entry ent, dent = {0};
      for (uint32_t i = 0; i < hdr.entry_num; ++i) {
        if (ctx->cancelled()) return ParserErr::SUCCESS;

        trfile.seekg(sizeof(trp_header) + (sizeof(trp_entry) * i));
        if (!trfile.read((char*)&ent, sizeof(trp_entry))) return ParserErr::IO_FAIL;

        ent.pos = _byteswap_uint64(ent.pos);
        ent.len = _byteswap_uint64(ent.len);

        auto ret = TRP_readentry(ent, dent, trfile, ctx, label);
        if (ret != ParserErr::CONTINUE) return ret;
      } // entries loop

      // Group or trophy callback is not cancelled yet, looks like we missed localized esfm file, trying to use the default one
      if (!ctx->group.cancelled || !ctx->entry.cancelled || !ctx->itrop.cancelled) {
        m_bIgnoreMissingLocale = true;
        if (dent.len != 0) {
          auto ret = TRP_readentry(dent, dent, trfile, ctx, label);
          if (ret != ParserErr::CONTINUE) return ret;
        }
      }

      return ParserErr::SUCCESS;
    } // trfile.is_open()

    return ParserErr::NO_TROPHIES;
  }

  const char* getError(ParserErr ec) final {
    switch (ec) {
      case ParserErr::SUCCESS: return "No errors";
      case ParserErr::INVALID_CONTEXT: return "Passed context is nullptr";
      case ParserErr::OUT_OF_MEMORY: return "Failed to allocate data array";
      case ParserErr::NO_KEY_SET: return "Invalid trophy key, decrypting is impossible";
      case ParserErr::INVALID_MAGIC: return "Invalid trophy file magic, trophy00.trp is likely corruted";
      case ParserErr::INVALID_VERSION: return "Unsupported trophy file version";
      case ParserErr::INVALID_ENTSIZE: return "Invalid trophy file entry size, trophy00.trp is likely corruted";
      case ParserErr::INVALID_AES: return "Trophy file contains unaligned AES blocks";
      case ParserErr::INVALID_XML: return "TPR file contains invalid XML data, can't parse it";
      case ParserErr::NOT_IMPLEMENTED: return "This feature is not implemented yet";
      case ParserErr::IO_FAIL: return "Your operating system reported IO failure";
      case ParserErr::NO_CALLBACKS: return "No callbacks passed to parser";
      case ParserErr::DECRYPT: return "Trophy file decryption failed";
      case ParserErr::NO_TROPHIES: return "Trophy file is likely missing or does not contain requested esfm file";
      case ParserErr::MAX_TROPHY_REACHED: return "The game hit the hard limit of 128 trophies";
      default: return "Unknown error code!";
    }
  }

  int32_t loadTrophyData(usr_context* ctx) {
    if (!ctx) return Err::NpTrophy::INVALID_CONTEXT;

    auto path = accessFileManager().getGameFilesDir() / std::format("tropinfo.{}-{}", ctx->userId, ctx->label);

    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (file.is_open()) {
      uint32_t tc = 0;

      if (!file.read((char*)&tc, 4)) return Err::NpTrophy::UNKNOWN;
      for (uint32_t i = 0; i < tc; i++) {
        usr_context::trophy t;
        if (!file.read((char*)&t.id, 4)) return Err::NpTrophy::UNKNOWN;
        if (!file.read((char*)&t.ts, 8)) return Err::NpTrophy::UNKNOWN;
        ctx->trophies.push_back(t);
      }
    }

    return Ok;
  }

  int32_t saveTrophyData(usr_context* ctx) {
    if (!ctx) return Err::NpTrophy::INVALID_CONTEXT;

    auto path = accessFileManager().getGameFilesDir() / std::format("tropinfo.{}-{}", ctx->userId, ctx->label);

    std::ofstream file(path, std::ios::out | std::ios::binary);

    if (file.is_open()) {
      uint32_t num = ctx->trophies.size();
      if (!file.write((char*)&num, 4)) return Err::NpTrophy::INSUFFICIENT_SPACE;

      for (auto& trop: ctx->trophies) {
        if (!file.write((char*)&trop.id, 4)) return Err::NpTrophy::INSUFFICIENT_SPACE;
        if (!file.write((char*)&trop.ts, 8)) return Err::NpTrophy::INSUFFICIENT_SPACE;
      }

      return Ok;
    }

    return Err::NpTrophy::INSUFFICIENT_SPACE;
  }

  usr_context* getContext(int32_t ctxid) final {
    for (auto it = m_ctx.begin(); it != m_ctx.end(); ++it) {
      if (it->label == (ctxid & 0xffff) && it->userId == ((ctxid >> 16) & 0xffff)) {
        return it._Ptr;
      }
    }

    return nullptr;
  }

  int32_t createContext(int32_t userId, uint32_t label, int32_t* ctxid) final {
    if (label > 99) return Err::NpTrophy::INVALID_NP_SERVICE_LABEL;

    if (userId < 1 || userId > 4) {
      *ctxid = -1;
      return Err::NpTrophy::INVALID_USER_ID;
    }

    for (auto it = m_ctx.begin(); it != m_ctx.end(); ++it) {
      if (it->label == label && it->userId == userId) {
        *ctxid = (((it->userId & 0xffff) << 16) | (it->label & 0xffff));
        return Err::NpTrophy::CONTEXT_ALREADY_EXISTS;
      }
    }

    if (m_ctx.size() == 8) return Err::NpTrophy::CONTEXT_EXCEEDS_MAX;

    struct usr_context ctx {
      .label = label, .userId = userId,
    };

    int32_t errcode;
    if ((errcode = loadTrophyData(&ctx)) == Ok) {
      checkPlatinumTrophies(&ctx);

      *ctxid = (((userId & 0xffff) << 16) | (label & 0xffff));
      m_ctx.push_back(std::move(ctx));

      return Ok;
    }

    *ctxid = -1;
    return errcode;
  }

  int32_t destroyContext(usr_context* ctx) final {
    if (!saveTrophyData(ctx)) return Err::NpTrophy::INSUFFICIENT_SPACE;

    for (auto it = m_ctx.begin(); it != m_ctx.end(); ++it) {
      if (ctx == it._Ptr) {
        ctx->label = -1;
        m_ctx.erase(it);
        return Ok;
      }
    }

    return Err::NpTrophy::INVALID_CONTEXT;
  }

  int32_t getProgress(usr_context* ctx, uint32_t progress[4], uint32_t* count) final {
    if (!ctx) return Err::NpTrophy::INVALID_CONTEXT;

    if (count != nullptr) {
      trp_context tctx = {
          .lightweight = true,
          .itrop =
              {
                  .func = [count](trp_inf_cb::data_t* data) -> bool {
                    *count = data->trophy_count;
                    return true;
                  },
              },
      };

      if (parseTRP(ctx->label, &tctx) != ParserErr::SUCCESS) return Err::NpTrophy::BROKEN_DATA;
    }

    SCE_NP_TROPHY_FLAG_ZERO((SceNpTrophyFlagArray*)progress);
    for (auto& trop: ctx->trophies) {
      SCE_NP_TROPHY_FLAG_SET(trop.id, (SceNpTrophyFlagArray*)progress);
    }

    return true;
  }

  uint64_t getUnlockTime(usr_context* ctx, int32_t trophyId) final {
    if (!ctx) return 0ull;

    for (auto& trop: ctx->trophies) {
      if (trop.id == trophyId) return trop.ts;
    }

    return 0ull;
  }

  int32_t getPlatinumIdFor(usr_context* ctx, int32_t trophyId) {
    if (!ctx) return -4;
    if (trophyId < 0 || trophyId > 128) return -3;

    int32_t platId = -1;

    trp_context tctx = {
        .lightweight = true,
        .entry =
            {
                .func = [&platId, trophyId](trp_ent_cb::data_t* data) -> bool {
                  if (data->id == trophyId) {
                    platId = (data->grade == 'p') ? -2 : data->platinum;
                    return true;
                  }

                  return false;
                },
            },
    };

    if (parseTRP(ctx->label, &tctx) != ParserErr::SUCCESS) platId = -3;
    return platId;
  }

  int32_t unlockTrophy(usr_context* ctx, int32_t trophyId, int32_t* platinumId) final {
    if (!ctx) return Err::NpTrophy::INVALID_CONTEXT;
    if (getUnlockTime(ctx, trophyId) > 0ull) return Err::NpTrophy::ALREADY_UNLOCKED;

    *platinumId = -1;
    int32_t errcode;

    if ((errcode = _unlockTrophyEx(ctx, trophyId, platinumId)) != Ok) {
      if (errcode == -1) { // No trophy decryption key
        trp_unlock_data cbdata = {
            .grade = 'b',
            .name  = "Unknown trophy",
            .descr = "No trophy key specified, can't decrypt the trophy data",
        };

        callUnlockCallback(&cbdata);
        errcode = Ok;
      }
    }

    uint64_t uTime = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::utc_clock::now().time_since_epoch()).count();

    ctx->trophies.push_back({trophyId, 0, uTime});
    if (*platinumId != -1) ctx->trophies.push_back({*platinumId, 0, uTime});

    return saveTrophyData(ctx) ? errcode : Err::NpTrophy::INSUFFICIENT_SPACE;
  }

  int32_t resetUserInfo(usr_context* ctx) final {
    if (!ctx) return Err::NpTrophy::INVALID_CONTEXT;

    ctx->trophies.clear();
    return true;
  }
};

ITrophies& accessTrophies() {
  static Trophies ti;
  return ti;
}
