#include "common.h"
#include "logging.h"
#include "png.h"
#include "types.h"

LOG_DEFINE_MODULE(libScePngDec);

namespace {
struct _PngHandle {
  png_structp png;
  png_infop   info;
};

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
  return ScePngDecColorSpace::INVALID;
}
} // namespace

extern "C" {

EXPORT const char* MODULE_NAME = "libScePngDec";

EXPORT SYSV_ABI int32_t scePngDecCreate(const ScePngDecCreateParam* param, void* mem, uint32_t size, ScePngDecHandle* handle) {
  if (mem == nullptr) return Err::INVALID_ADDR;
  if (size < sizeof(_PngHandle)) return Err::INVALID_SIZE;
  auto pngh = (_PngHandle*)mem;

  pngh->png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (pngh->png == nullptr) return Err::FATAL;

  pngh->info = png_create_info_struct(pngh->png);
  if (pngh->info == nullptr) {
    png_destroy_read_struct(&pngh->png, nullptr, nullptr);
    return false;
  }

  *handle = pngh;
  return Ok;
}

EXPORT SYSV_ABI int32_t scePngDecDecode(ScePngDecHandle handle, const ScePngDecDecodeParam* param, ScePngDecImageInfo* ii) {
  LOG_USE_MODULE(libScePngDec);
  if (param->pngAddr == nullptr || param->imageAddr == nullptr) return Err::INVALID_ADDR;
  if (param->pngSize < 8) return Err::INVALID_SIZE;
  if (png_sig_cmp((png_const_bytep)param->pngAddr, 0, 8) != 0) return Err::INVALID_DATA;
  if (handle == nullptr) return Err::INVALID_HANDLE;
  auto pngh = (_PngHandle*)handle;

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
  png_set_read_fn(pngh->png, (void*)&pngrd, [](png_structp ps, png_bytep data, png_size_t len) {
    if (len == 0) return;
    auto pngrd = (pngreader*)png_get_io_ptr(ps);
    if (pngrd->offset + len > pngrd->size) return png_error(ps, "Read error!");
    ::memcpy(data, pngrd->data + pngrd->offset, len);
    pngrd->offset += len;
  });

  uint32_t w, h;
  int      ct, bi, in;
  png_read_info(pngh->png, pngh->info);
  png_get_IHDR(pngh->png, pngh->info, &w, &h, &bi, &ct, &in, nullptr, nullptr);

  if (ii != nullptr) {
    ii->bitDepth    = bi;
    ii->imageWidth  = w;
    ii->imageHeight = h;
    ii->imageFlag   = 0;
    ii->colorSpace  = map_png_color(ct);
    if (in == 1) ii->imageFlag |= SCE_PNG_DEC_IMAGE_FLAG_ADAM7_INTERLACE;
    if (png_get_valid(pngh->png, pngh->info, PNG_INFO_tRNS)) ii->imageFlag |= SCE_PNG_DEC_IMAGE_FLAG_TRNS_CHUNK_EXIST;
  }

  if ((w * h * 4) > param->imageSize) return Err::INVALID_SIZE;
  if (bi == 16) png_set_strip_16(pngh->png);
  if (ct == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(pngh->png);
  if (ct == PNG_COLOR_TYPE_GRAY && bi < 8) png_set_expand_gray_1_2_4_to_8(pngh->png);
  if (png_get_valid(pngh->png, pngh->info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(pngh->png);
  if (ct == PNG_COLOR_TYPE_GRAY || ct == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(pngh->png);
  if (param->pixelFormat == ScePngDecPixelFormat::B8G8R8A8) png_set_bgr(pngh->png);
  if (ct == PNG_COLOR_TYPE_RGB) png_set_add_alpha(pngh->png, 0xFF, PNG_FILLER_AFTER);
  png_read_update_info(pngh->png, pngh->info);

  png_bytepp row_ptr = (png_bytep*)png_malloc(pngh->png, sizeof(png_bytep) * h);
  for (int y = 0; y < h; y++) {
    row_ptr[y] = (png_byte*)png_calloc(pngh->png, png_get_rowbytes(pngh->png, pngh->info));
  }
  png_read_image(pngh->png, row_ptr);

  auto ptr = (png_bytep)param->imageAddr;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w * 4 /*RGBA*/; x++) {
      *ptr++ = row_ptr[y][x];
    }
    png_free(pngh->png, row_ptr[y]);
  }
  png_free(pngh->png, row_ptr);

  return (w > 32767 || h > 32767) ? 0 : ((uint32_t)w << 16) | (uint32_t)h;
}

EXPORT SYSV_ABI int32_t scePngDecDelete(ScePngDecHandle handle) {
  if (handle == nullptr) return Err::INVALID_HANDLE;
  auto pngh = *(_PngHandle**)handle;
  png_destroy_read_struct(&pngh->png, &pngh->info, nullptr);
  return Ok;
}

EXPORT SYSV_ABI int32_t scePngDecParseHeader(const ScePngDecParseParam* param, ScePngDecImageInfo* ii) {
  if (param->pngAddr == nullptr) return Err::INVALID_ADDR;
  if (param->pngSize < 8) return Err::INVALID_SIZE;
  if (png_sig_cmp((png_const_bytep)param->pngAddr, 0, 8) != 0) return Err::INVALID_DATA;

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

  int bitdepth, color, filter, inter;
  png_get_IHDR(png_ptr, info_ptr, &ii->imageWidth, &ii->imageHeight, &bitdepth, &color, &inter, nullptr, &filter);

  ii->imageFlag  = 0;
  ii->bitDepth   = bitdepth;
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
  return sizeof(_PngHandle);
}
}
