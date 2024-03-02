#pragma once
#include "utility/utility.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <string>

enum class MountType { App, Save, Temp, Data };
constexpr int FILE_DESCRIPTOR_MIN = 3;

constexpr std::string_view MOUNT_POINT_TEMP = "temp";
constexpr std::string_view MOUNT_POINT_DATA = "data";
constexpr std::string_view MOUNT_POINT_APP  = "/app0/";
constexpr std::string_view SAVE_DATA_POINT  = "/savedata";

class IFileManager {
  CLASS_NO_COPY(IFileManager);
  CLASS_NO_MOVE(IFileManager);

  protected:
  IFileManager()          = default;
  virtual ~IFileManager() = default;

  public:
  /**
   * @brief Get the mapped path. (Resolves linux paths, mounts etc.)
   *
   * @param path
   * @return std::optional<std::filesystem::path> on success, the mapped path
   */
  virtual std::optional<std::filesystem::path> getMappedPath(std::string_view path) = 0;

  /**
   * @brief Adds a fake mount point. getMappedPath() uses those to resolve to the correct path.
   * Just replaces both strings
   *
   * @param mountPoint e.g. /savedata
   * @param root  actual path e.g. C:/savedir
   * @param type Used to group the mounts. normaly only one mount per type is used. (savedata uses multiple ...)
   */
  virtual void addMountPoint(std::string_view mountPoint, std::filesystem::path const& root, MountType type) = 0;

  /**
   * @brief Get the actual path for a mount point
   *
   * @param type
   * @param mountPoint e.g. /savedata
   * @return std::filesystem::path  empty on error
   */
  virtual std::filesystem::path getMountPoint(MountType type, std::string mountPoint) = 0;

  /**
   * @brief Removes the mountpoint
   *
   * @param type
   * @param mountPoint
   */
  virtual void clearMountPoint(MountType type, std::string mountPoint) = 0;

  /**
   * @brief Sets the path where all games files should be located (savegames etc).
   * Used by main to setup the emulator
   * @param path
   */
  virtual void setGameFilesDir(std::filesystem::path const& path) = 0;

  /**
   * @brief Get the Game Files Dir
   *
   * @return std::filesystem::path const&
   */
  virtual std::filesystem::path const& getGameFilesDir() const = 0;

  /**
   * @brief Add a open fstream.
   *
   * @param file
   * @param path the mapped path to the file/folder
   * @return int handle of the fstream. used by getFile() etc.
   */
  virtual int addFileStream(std::unique_ptr<std::fstream>&& file, std::filesystem::path const& path) = 0;

  /**
   * @brief Add a directory_iterator
   *
   * @param it
   * @param path the mapped path to the folder
   * @return int handle to the directory_iterator
   */
  virtual int addDirIterator(std::unique_ptr<std::filesystem::directory_iterator>&& it, std::filesystem::path const& path) = 0;

  /**
   * @brief remove the file/folder with the associated handle.
   * Only the internal mapping is removed!
   *
   * @param handle
   */
  virtual void remove(int handle) = 0;

  /**
   * @brief Get the File
   *
   * @param handle
   * @return std::fstream*
   */
  virtual std::fstream* getFile(int handle) = 0;

  virtual int getDents(int handle, char* buf, int nbytes, int64_t* basep) = 0;

  /**
   * @brief Get the mapped path of a open file/folder
   *
   * @param handle
   * @return std::filesystem::path
   */
  virtual std::filesystem::path getPath(int handle) = 0;
};

#if defined(__APICALL_EXTERN)
#define __APICALL __declspec(dllexport)
#elif defined(__APICALL_IMPORT)
#define __APICALL __declspec(dllimport)
#else
#define __APICALL
#endif
__APICALL IFileManager& accessFileManager();
#undef __APICALL