#ifndef N64GRAPHICS_CI_H_
#define N64GRAPHICS_CI_H_

#include <stdint.h>

// intermediate formats
typedef struct _ngci_rgba
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} ngci_rgba;

typedef struct _ngci_ia
{
    uint8_t intensity;
    uint8_t alpha;
} ngci_ia;

// N64 raw RGBA16/RGBA32 -> intermediate RGBA
ngci_rgba *raw2rgba_ngci(const uint8_t *raw, int width, int height, int depth);

// N64 raw CI + palette -> intermediate RGBA
ngci_rgba *rawci2rgba(const uint8_t *rawci, const uint8_t *palette, int width, int height, int depth);

// intermediate RGBA -> N64 raw CI + palette
int rgba2rawci(uint8_t *raw, uint8_t *out_palette, int *pal_len, const ngci_rgba *img, int width, int height, int depth);

// PNG file -> intermediate RGBA
ngci_rgba *png2rgba_ngci(const char *png_filename, int *width, int *height);

// intermediate RGBA write to PNG file
int rgba2png_ngci(const char *png_filename, const ngci_rgba *img, int width, int height);

// get version of underlying graphics reading library
const char *n64graphics_ci_get_read_version(void);

// get version of underlying graphics writing library
const char *n64graphics_ci_get_write_version(void);

#endif
