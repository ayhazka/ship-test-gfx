
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>

#include "n64graphics_ci_dir/n64graphics_ci.h"
#include "n64graphics.h"
#include "utils.h"

#define TICONV_ENCODING ENCODING_U8

typedef struct
{
    enum
    {
       IMG_FORMAT_RGBA,
       IMG_FORMAT_IA,
       IMG_FORMAT_I,
       IMG_FORMAT_CI,
    } format;
    int depth;
} img_format;

void write_titexdata_c(FILE *fp, const char* tex_name, uint8_t* img, int img_size, uint8_t* pal, int pal_size, int width, int height, img_format* format, int block) {
    if (format->format == IMG_FORMAT_CI)
        fprintf(fp, "extern u8 %s_tlut[];\n", tex_name);
    fprintf(fp, "extern u8 %s_tex[];\n", tex_name);

    fprintf(fp, "TexturePtr %s_texs[2] = { ", tex_name);
    if (format->format == IMG_FORMAT_CI)
        fprintf(fp, "%s_tlut, ", tex_name);
    else
        fprintf(fp, "0, ");
    fprintf(fp, "%s_tex };\n", tex_name);

    fprintf(fp, "u16 %s_info[4] = { %d, %d, %d, %d };\n\n",
            tex_name, width, height, format->depth, block);

    if (format->format == IMG_FORMAT_CI) {
        fprintf(fp, "u8 %s_tlut[] = {\n", tex_name);
        fprint_write_output(fp, TICONV_ENCODING, pal, pal_size);
        fprintf(fp, "};\n\n");
    }
    fprintf(fp, "u8 %s_tex[] = {\n", tex_name);
    fprint_write_output(fp, TICONV_ENCODING, img, img_size);
    fprintf(fp, "};\n\n");
}

typedef struct
{
   const char *name;
   img_format format;
} format_entry;

static const format_entry format_table[] =
{
   {"rgba16", {IMG_FORMAT_RGBA, 16}},
   //{"rgba32", {IMG_FORMAT_RGBA, 32}},
   //{"ia1",    {IMG_FORMAT_IA,    1}},
   //{"ia4",    {IMG_FORMAT_IA,    4}},
   //{"ia8",    {IMG_FORMAT_IA,    8}},
   //{"ia16",   {IMG_FORMAT_IA,   16}},
   {"i4",     {IMG_FORMAT_I,     4}},
   {"i8",     {IMG_FORMAT_I,     8}},
   {"ci8",    {IMG_FORMAT_CI,    8}},
   {"ci4",    {IMG_FORMAT_CI,    4}},
};

static const char *format2str(const img_format *format)
{
   for (unsigned i = 0; i < DIM(format_table); i++) {
      if (format->format == format_table[i].format.format && format->depth == format_table[i].format.depth) {
         return format_table[i].name;
      }
   }
   return "unknown";
}

static int parse_format(img_format *format, const char *str)
{
   for (unsigned i = 0; i < DIM(format_table); i++) {
      if (!strcasecmp(str, format_table[i].name)) {
         format->format = format_table[i].format.format;
         format->depth = format_table[i].format.depth;
         return 1;
      }
   }
   return 0;
}

typedef struct
{
   char *img_filename;
   char *out_filename;
   img_format format;
   int width;
   int height;
   int quantize_ci;
   int block;
} ticonv_config;

static const ticonv_config default_config =
{
   .img_filename = NULL,
   .out_filename = NULL,
   .format = {IMG_FORMAT_RGBA, 16},
   .width = 32,
   .height = 32,
   .quantize_ci = 0,
   .block = 1,
};

#define PROGRAM_OPTIONS "f:i:o:qvht"

static struct option prg_long_options[] = {
    {"format", required_argument, 0, 'f'},
    {"input",  required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'},
    {"help",   no_argument,       0, 'h'},
    {"quant",  no_argument,       0, 'q'},
    {"tile",   no_argument,       0, 't'},
    {0, 0, 0, 0},
};

static void print_usage(void) {
    ERROR("Usage: ticonv -i INPUT -o OUTPUT [-f FORMAT] [-q] [-v] [-t] [-h]\n");
}

static void parse_args(int argc, char* argv[], ticonv_config* config) {
    int opt;
    int opt_idx;

    while ((opt = getopt_long(argc, argv, PROGRAM_OPTIONS, prg_long_options, &opt_idx)) != -1) {
        switch (opt) {
            case 'f':
                if (!parse_format(&config->format, optarg)) {
                    ERROR("Invalid format or not supported: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'i':
                config->img_filename = optarg;
                break;
            case 'o':
                config->out_filename = optarg;
                break;
            case 'q':
                config->quantize_ci = 1;
                break;
            case 't':
                config->block = 0;
                break;
            case 'h':
                print_usage();
                exit(0);
                break;
            case 'v':
                g_verbosity = 1;
                break;
            case '?':
                exit(EXIT_FAILURE);
                break;
        }
    }
}

static void validate_config(ticonv_config* config) {
    if (!config->img_filename || !config->out_filename) {
        ERROR("No input & output file specified\n");
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    ticonv_config config = default_config;
    FILE *out_fp;
    rgba *imgr;
    ia *imgi;
    uint8_t *raw;
    int raw_size;
    int length;

    uint8_t *raw_pal = NULL;
    int pal_size = 0;

    parse_args(argc, argv, &config);
    validate_config(&config);

    switch(config.format.format) {
        case IMG_FORMAT_RGBA:
            imgr = png2rgba(config.img_filename, &config.width, &config.height);
            raw_size = (config.width * config.height * config.format.depth + 7) / 8;
            raw = malloc(raw_size);
            if (!raw) {
               ERROR("Error allocating %u bytes\n", raw_size);
            }
            length = rgba2raw(raw, imgr, config.width, config.height, config.format.depth);
            break;
        case IMG_FORMAT_I:
            imgi = png2ia(config.img_filename, &config.width, &config.height);
            raw_size = (config.width * config.height * config.format.depth + 7) / 8;
            raw = malloc(raw_size);
            if (!raw) {
               ERROR("Error allocating %u bytes\n", raw_size);
            }
            length = i2raw(raw, imgi, config.width, config.height, config.format.depth);
            break;
        case IMG_FORMAT_CI:
            imgr = png2rgba(config.img_filename, &config.width, &config.height);
            raw_size = config.width * config.height * config.format.depth / 8;

            if (config.quantize_ci) {
                pal_size = (1 << config.format.depth);
                raw = malloc(raw_size);
                if (!raw) {
                    ERROR("Error allocating %d bytes\n", raw_size);
                    return EXIT_FAILURE;
                }
                raw_pal = malloc(pal_size);
                if (!raw_pal) {
                    ERROR("Error allocating %u bytes for palette\n", pal_size);
                }
                length = rgba2rawci(raw, raw_pal, &pal_size, imgr, config.width, config.height, config.format.depth);
            }
            else {
                palette_t pal = {0};
                uint8_t *raw16;
                int raw16_size;
                int raw16_length;

                raw16_size = config.width * config.height * 16 / 8;
                raw16 = malloc(raw16_size);
                if (!raw16) {
                    ERROR("Error allocating %d bytes\n", raw16_size);
                    return EXIT_FAILURE;
                }
                raw16_length = rgba2raw(raw16, imgr, config.width, config.height, 16);

                // convert raw to palette
                pal.max = (1 << config.format.depth);
                raw = malloc(raw_size);
                if (!raw) {
                    ERROR("Error allocating %d bytes\n", raw_size);
                    return EXIT_FAILURE;
                }
                length = raw2ci(raw, &pal, raw16, raw16_length, config.format.depth);
                if (length <= 0) {
                    ERROR("Error converting palette or too big to fit in. Try using --quant to make the palette smaller.\n");
                    return EXIT_FAILURE;
                }

                // pack the bytes
                raw_pal = malloc(pal.max);
                if (!raw_pal) {
                    ERROR("Error allocating %d bytes for palette\n", pal.max);
                    return EXIT_FAILURE;
                }
                for (int i = 0; i < pal.max; i++) {
                    write_u16_be(&raw_pal[2*i], pal.data[i]);
                }
                pal_size = pal.max * sizeof(pal.data[0]);

                free(raw16);
            }
            break;
        default:
            return EXIT_FAILURE;
    }
    if (length <= 0) {
        ERROR("Error converting to raw format\n");
        return EXIT_FAILURE;
    }

    char tex_name[PATH_MAX];
    if (0 == strcmp("-", config.out_filename)) {
        out_fp = stdout;
        strcpy(tex_name, "_stdout");
    } else {
        out_fp = fopen(config.out_filename, "wb");

        char *base = basename(config.out_filename);
        strcpy(tex_name, base);
        char *extension = strrchr(tex_name, '.');
        if (extension) *extension = '\0';
    }
    if (!out_fp) {
        ERROR("Error opening \"%s\"\n", config.out_filename);
        return -1;
    }

    write_titexdata_c(out_fp, tex_name, raw, raw_size, raw_pal, pal_size,
                    config.width, config.height, &config.format, config.block);

    free(raw);
    if (raw_pal) free(raw_pal);
    return 0;

}
