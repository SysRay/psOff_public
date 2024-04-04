#include "common.h"
#include "logging.h"
#include "png.h"
#include "types.h"

LOG_DEFINE_MODULE(libScePngDec);

namespace {
static inline ScePngDecColorSpace map_png_color(int color) {
  LOG_USE_MODULE(libScePngDec);

  switch (color) {
    case PNG_COLOR_TYPE_GRAY: return ScePngDecColorSpace::GRAYSCALE;

    case PNG_COLOR_TYPE_GRAY_ALPHA: return ScePngDecColorSpace::GRAYSCALE_ALPHA;

    case PNG_COLOR_TYPE_PALETTE: return ScePngDecColorSpace::CLUT;

    case PNG_COLOR_TYPE_RGB: return ScePngDecColorSpace::RGB;

    case PNG_COLOR_TYPE_RGB_ALPHA: return ScePngDecColorSpace::RGBA;
  }

  LOG_CRIT(L"Unknown png color type");
  return ScePngDecColorSpace::RGB;
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libScePngDec";

EXPORT SYSV_ABI int32_t scePngDecCreate(const ScePngDecCreateParam*, void*, uint32_t, ScePngDecHandle* handle) {
  *handle = nullptr;
  return Ok;
}

EXPORT SYSV_ABI int32_t scePngDecDecode(ScePngDecHandle handle, const ScePngDecDecodeParam* param, ScePngDecImageInfo* imageInfo) {
  LOG_USE_MODULE(libScePngDec);

  if (param->pngAddr == nullptr || param->imageAddr == nullptr) return Err::INVALID_ADDR;
  if (param->pngSize == 0 || param->imageSize == 0) return Err::INVALID_SIZE;

  auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) return false;

  auto info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return false;
  }

  struct pngreader {
    const uint8_t* data;
    size_t         size;
    uint64_t       offset;
  } pngrd = {
      .data   = (const uint8_t*)param->pngAddr,
      .size   = param->pngSize,
      .offset = 0,
  };

  // Read png from memory: param->pngAddr, param->pngSize
  png_set_read_fn(png_ptr, (void*)&pngrd, [](png_structp ps, png_bytep data, png_size_t len) {
    if (len == 0) return;
    auto pngrd = (pngreader*)png_get_io_ptr(ps);
    if (pngrd->offset + len > pngrd->size) return png_error(ps, "Read error!");
    ::memcpy(data, pngrd->data + pngrd->offset, len);
    pngrd->offset += len;
  });

  uint32_t w, h;
  int      ct, bi;
  png_read_info(png_ptr, info_ptr);
  png_get_IHDR(png_ptr, info_ptr, &w, &h, &bi, &ct, nullptr, nullptr, nullptr);
  if (bi == 16) png_set_strip_16(png_ptr);
  if (ct == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
  if (ct == PNG_COLOR_TYPE_GRAY && bi < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
  if (ct == PNG_COLOR_TYPE_GRAY || ct == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png_ptr);
  if (param->pixelFormat == ScePngDecPixelFormat::B8G8R8A8) png_set_bgr(png_ptr);
  png_read_update_info(png_ptr, info_ptr);

  png_bytepp row_ptr = (png_bytep*)png_malloc(png_ptr, sizeof(png_bytep) * h);
  for (int y = 0; y < h; y++) {
    row_ptr[y] = (png_byte*)png_malloc(png_ptr, png_get_rowbytes(png_ptr, info_ptr));
  }
  png_read_image(png_ptr, row_ptr);

  auto ptr = (png_bytep)param->imageAddr;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w * 4 /*RGBA*/; x++) {
      *ptr++ = row_ptr[y][x];
    }
    png_free(png_ptr, row_ptr[y]);
  }
  png_free(png_ptr, row_ptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePngDecDelete(ScePngDecHandle handle) {
  return Ok;
}

EXPORT SYSV_ABI int32_t scePngDecParseHeader(const ScePngDecParseParam* param, ScePngDecImageInfo* ii) {
  if (param->pngAddr == nullptr) return Err::INVALID_ADDR;
  if (param->pngSize < 8) return Err::INVALID_SIZE;

  auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) return false;

  auto info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return false;
  }

  struct pngreader {
    const uint8_t* data;
    size_t         size;
    uint64_t       offset;
  } pngrd = {
      .data   = (const uint8_t*)param->pngAddr,
      .size   = param->pngSize,
      .offset = 0,
  };

  png_set_read_fn(png_ptr, (void*)&pngrd, [](png_structp ps, png_bytep data, png_size_t len) {
    auto pngrd = (pngreader*)png_get_io_ptr(ps);
    if (pngrd->offset + len > pngrd->size) return png_error(ps, "Read error!");
    ::memcpy(data, pngrd->data + pngrd->offset, len);
    pngrd->offset += len;
  });

  png_read_info(png_ptr, info_ptr);

  int bitdepth, color, filter, inter, num_trans;

  png_get_IHDR(png_ptr, info_ptr, &ii->imageWidth, &ii->imageHeight, &bitdepth, &color, &inter, nullptr, &filter);

  ii->colorSpace = map_png_color(color);
  if (inter == 1) ii->imageFlag |= SCE_PNG_DEC_IMAGE_FLAG_ADAM7_INTERLACE;
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) ii->imageFlag |= SCE_PNG_DEC_IMAGE_FLAG_TRNS_CHUNK_EXIST;
  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePngDecQueryMemorySize(const ScePngDecCreateParam* param) {
  if (param == nullptr) return Err::INVALID_ADDR;
  if (param->cbSize != sizeof(ScePngDecCreateParam)) return Err::INVALID_SIZE;
  if (param->maxImageWidth > 1000000) return Err::INVALID_PARAM;
  return 0;
}
}
