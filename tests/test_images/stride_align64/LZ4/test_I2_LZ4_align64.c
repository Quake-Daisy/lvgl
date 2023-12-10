
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_BUILD_TEST)
#include "../lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_DUST
#define LV_ATTRIBUTE_IMG_DUST
#endif

static const
LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_DUST
uint8_t test_I2_LZ4_align64_map[] = {

    0x02,0x00,0x00,0x00,0x52,0x02,0x00,0x00,0x90,0x07,0x00,0x00,0xfd,0x01,0x5a,0x00,
    0x3d,0xf4,0x23,0xaf,0xaf,0x08,0x60,0xff,0xf7,0xdf,0x37,0xff,0x18,0xff,0x01,0x00,
    0x29,0xfc,0x00,0x01,0x00,0x2b,0xd5,0x55,0x01,0x00,0x19,0x5c,0x1f,0x00,0x0f,0x20,
    0x00,0x0e,0x53,0xd4,0x00,0x02,0xaa,0xa9,0x43,0x00,0x5b,0x54,0x00,0x00,0x00,0x00,
    0x40,0x00,0x02,0x20,0x00,0xbf,0x54,0x00,0x15,0x55,0x55,0x55,0x54,0x55,0x55,0x55,
    0x54,0x20,0x00,0x02,0xbf,0x40,0x00,0x01,0x55,0x54,0x15,0x54,0x40,0x00,0x00,0x04,
    0x20,0x00,0x02,0xbf,0x00,0x00,0x00,0x55,0x50,0x05,0x54,0x45,0x55,0x55,0x44,0x20,
    0x00,0x01,0xbf,0x54,0x00,0x00,0x00,0x15,0x50,0x05,0x54,0x44,0x00,0x00,0x20,0x00,
    0x07,0x6f,0x54,0x15,0x54,0x44,0x55,0x54,0x20,0x00,0x02,0xbf,0x50,0x00,0x00,0x00,
    0x05,0x55,0x55,0x54,0x44,0x40,0x04,0x20,0x00,0x0b,0x2c,0x45,0x44,0x20,0x00,0x51,
    0xd6,0xaa,0xa8,0x00,0x01,0x40,0x00,0x5f,0x50,0x05,0x54,0x44,0x44,0x20,0x00,0x08,
    0x2f,0x40,0x01,0x40,0x00,0x0b,0x2f,0x00,0x00,0x80,0x00,0x01,0x06,0x60,0x00,0x2f,
    0x00,0x00,0xc0,0x00,0x01,0x01,0x20,0x00,0x01,0x00,0x01,0x2f,0x00,0x00,0x00,0x01,
    0x01,0x09,0x20,0x00,0x0f,0x40,0x01,0x00,0x01,0x20,0x00,0x7f,0x55,0x00,0x00,0x00,
    0x55,0x40,0x01,0x80,0x01,0x01,0x02,0x20,0x00,0x6f,0x40,0x00,0x01,0x55,0x50,0x05,
    0xc0,0x01,0x01,0x02,0x20,0x02,0x03,0xe0,0x01,0x0f,0x00,0x02,0x00,0x02,0x20,0x00,
    0x07,0x02,0x00,0x0b,0xc0,0x01,0x0f,0x20,0x00,0x12,0x17,0x40,0x94,0x02,0x0c,0x40,
    0x00,0x37,0x50,0x00,0x05,0x5e,0x00,0x0e,0x60,0x00,0x3f,0x00,0x00,0x00,0x20,0x00,
    0x0d,0x18,0x15,0x21,0x00,0x0c,0x60,0x00,0x21,0x00,0x55,0xbc,0x02,0x05,0x02,0x00,
    0x0e,0x20,0x00,0x23,0x40,0x05,0x00,0x01,0x01,0x02,0x00,0x0c,0x20,0x00,0x81,0x01,
    0x55,0x40,0x05,0x55,0x50,0x00,0x01,0x1d,0x00,0x0f,0x20,0x00,0x00,0x6f,0x55,0x55,
    0x40,0x05,0x55,0x40,0x20,0x00,0x07,0x72,0x54,0x00,0x00,0x05,0x55,0x50,0x55,0xc5,
    0x00,0x3c,0x44,0x51,0x51,0x60,0x00,0x18,0x40,0x20,0x00,0x0f,0x60,0x00,0x00,0x02,
    0xdb,0x02,0x5f,0x40,0x00,0x55,0x55,0x50,0x20,0x00,0x01,0x31,0xd4,0x00,0x15,0xa0,
    0x00,0x23,0x00,0x55,0xe8,0x00,0x0b,0x60,0x00,0x22,0xd4,0x00,0xa0,0x00,0x5f,0x00,
    0x55,0x55,0x05,0x05,0x80,0x00,0x00,0x03,0x20,0x00,0x51,0x15,0x00,0x55,0x55,0x54,
    0x89,0x01,0x0d,0x40,0x00,0x92,0x14,0x00,0x05,0x55,0x40,0x55,0x00,0x55,0x55,0xa9,
    0x01,0x0b,0x20,0x00,0xbf,0xd5,0x00,0x00,0x00,0x01,0x55,0x40,0x04,0x00,0x55,0x54,
    0x60,0x00,0x02,0x21,0xd5,0x00,0x9c,0x02,0x5f,0x00,0x00,0x15,0x55,0x04,0x60,0x00,
    0x01,0xdf,0xd5,0x40,0x00,0x40,0x01,0x55,0x50,0x00,0x40,0x15,0x55,0x00,0x01,0xa0,
    0x00,0x00,0x0f,0x60,0x02,0x12,0x08,0x02,0x00,0x0b,0xa0,0x00,0x11,0xd7,0x41,0x05,
    0x8e,0xfe,0xaa,0xaa,0xaa,0xaa,0xaa,0xa9,0x99,0x60,0x01,0x02,0x20,0x00,0x11,0xfb,
    0x20,0x00,0x3f,0x9a,0x66,0x66,0x40,0x00,0x04,0x11,0xee,0x20,0x00,0x2f,0xa6,0x66,
    0x40,0x00,0x0b,0x4f,0xaa,0x99,0x99,0x95,0x80,0x00,0x03,0x02,0x40,0x00,0x1f,0x66,
    0x40,0x00,0x0c,0x4f,0xa9,0x99,0x99,0x65,0x40,0x00,0x09,0x1f,0x9a,0x40,0x00,0x0f,
    0x0f,0x80,0x00,0x2a,0x1f,0xaa,0x80,0x00,0x0c,0x1f,0x99,0x40,0x01,0x0c,0x0f,0x00,
    0x01,0x2d,0x2e,0xaa,0x99,0x80,0x01,0x0f,0x00,0x02,0x2d,0x0f,0x40,0x00,0x0d,0x01,
    0x1f,0x02,0x08,0x02,0x00,0x06,0x60,0x07,0x50,0x00,0x00,0x00,0x00,0x00,

};

const lv_img_dsc_t test_I2_LZ4_align64 = {
  .header.cf = LV_COLOR_FORMAT_I2,
  .header.flags = 0 | LV_IMAGE_FLAGS_COMPRESSED,
  .header.w = 71,
  .header.h = 60,
  .header.stride = 32,
  .data_size = 606,
  .data = test_I2_LZ4_align64_map,
};

