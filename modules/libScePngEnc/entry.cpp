#include "common.h"
#include "logging.h"
#include "png.h"
#include "types.h"

LOG_DEFINE_MODULE(libScePngEnc);

namespace {
struct _PngHandle {
  png_structp png;
  png_infop   info;
};
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libScePngEnc";

EXPORT SYSV_ABI int32_t scePngEncCreate(const ScePngEncCreateParam* param, void* mem, uint32_t size, ScePngEncHandle* handle) {
  if (mem == nullptr) return Err::PngEnc::INVALID_ADDR;
  if (size < sizeof(_PngHandle)) return Err::PngEnc::INVALID_SIZE;
  auto pngh = (_PngHandle*)mem;

  pngh->png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (pngh->png == nullptr) return Err::PngEnc::FATAL;

  pngh->info = png_create_info_struct(pngh->png);
  if (pngh->info == nullptr) {
    png_destroy_read_struct(&pngh->png, nullptr, nullptr);
    return false;
  }

  *handle = pngh;
  return Ok;
}

EXPORT SYSV_ABI int32_t scePngEncEncode(ScePngEncHandle handle, const ScePngEncEncodeParam* param, ScePngEncOutputInfo* oi) {
  LOG_USE_MODULE(libScePngEnc);
  if (param == nullptr || param->pngAddr == nullptr || param->imageAddr == nullptr) return Err::PngEnc::INVALID_ADDR;
  auto pngh = *(_PngHandle**)handle;
  return 0;
}

EXPORT SYSV_ABI int32_t scePngEncDelete(ScePngEncHandle handle) {
  if (handle == nullptr) return Err::PngEnc::INVALID_HANDLE;
  auto pngh = *(_PngHandle**)handle;
  png_destroy_read_struct(&pngh->png, &pngh->info, nullptr);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePngEncQueryMemorySize(const ScePngEncCreateParam* param) {
  if (param == nullptr) return Err::PngEnc::INVALID_ADDR;
  if (param->cbSize != sizeof(ScePngEncCreateParam)) return Err::PngEnc::INVALID_SIZE;
  if (param->maxImageWidth > 1000000) return Err::PngEnc::INVALID_PARAM;
  return sizeof(_PngHandle);
}
}
