#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <bmplib.h>
#include <string.h>

#define TILE_SIZE 8

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Usage: %s <input_file.bmp> <output_file.til>\n", argv[0]);
        return 1;
    }

    /*
        BMP parsing
    */

    FILE *fd_bmp = fopen(argv[1], "rb");
    if (fd_bmp == NULL)
    {
        perror("Failed to open input BMP file ");
        return 1;
    }

    FILE *fd_til = fopen(argv[2], "wb");
    if (fd_til == NULL)
    {
        fclose(fd_bmp);
        perror("Failed to open/create output til file ");
        return 1;
    }

    /*
        Open BMP and get info
    */
    BMPRESULT bmp_err;
    BMPHANDLE bmp_h = bmpread_new(fd_bmp);
    bmp_err = bmpread_load_info(bmp_h);

    if (bmp_err)
    {
        printf("Failed to get BMP info error: %d : %s\n", bmp_err, bmp_errmsg(bmp_h));
        bmp_free(bmp_h);
        fclose(fd_bmp);
        fclose(fd_til);
        return 1;
    }

    /*
        Get palette if there is any
    */

    unsigned char *bmp_palette;
    int bmp_pal_num_colors;

    bmp_pal_num_colors = bmpread_num_palette_colors(bmp_h);
    if (bmp_pal_num_colors == 0)
    {
        printf("%s is not indexd and isn't currently supported by this tool!\n", argv[1]);
        bmp_free(bmp_h);
        fclose(fd_bmp);
        fclose(fd_til);
        return 1;
    }

    if (bmp_pal_num_colors > 256)
    {
        printf("%s has too many palette colors!\nUp to 256 | Actual: %d", argv[1], bmp_pal_num_colors);
        bmp_free(bmp_h);
        fclose(fd_bmp);
        fclose(fd_til);
        return 1;
    }

    printf("INFO: %d colors in palette\n", bmp_pal_num_colors);

    /*
        Load palette
    */
    bmp_palette = malloc(4 * 256); // Always max palette
    bmp_err = bmpread_load_palette(bmp_h, &bmp_palette);
    if (bmp_err)
    {
        printf("Failed to load palette: %s\n", bmp_errmsg(bmp_h));
        bmp_free(bmp_h);
        free(bmp_palette);
        fclose(fd_bmp);
        fclose(fd_til);
        return 1;
    }

    /*
        Load image
    */

    int bmp_width;
    int bmp_height;
    int bmp_channels;
    int bmp_bitsperchannel;

    bmp_err = bmpread_dimensions(bmp_h, &bmp_width, &bmp_height, &bmp_channels, &bmp_bitsperchannel, NULL);
    if (bmp_err)
    {
        printf("Failed to read BMP dimensions: %s\n", bmp_errmsg(bmp_h));
        bmp_free(bmp_h);
        fclose(fd_bmp);
        fclose(fd_til);
        return 1;
    }
    /*
        Check info
    */
    if (bmp_width % TILE_SIZE != 0 && bmp_width > 0)
    {
        printf("%s is incorect width for selected tilite size of %d!\n", argv[1], TILE_SIZE);
        bmp_free(bmp_h);
        fclose(fd_bmp);
        fclose(fd_til);
        return 1;
    }
    if (bmp_height % TILE_SIZE != 0 && bmp_height > 0)
    {
        printf("%s is incorect height for selected tilite size of %d!\n", argv[1], TILE_SIZE);
        bmp_free(bmp_h);
        fclose(fd_bmp);
        fclose(fd_til);
        return 1;
    }

    if (((bmp_width / TILE_SIZE) * (bmp_height / TILE_SIZE)) > 1024) // Max 1024 tiles
    {
        printf("Max number of tiles per file is 1024, this file has %d\n", ((bmp_width / TILE_SIZE) * (bmp_height / TILE_SIZE)));
        bmp_free(bmp_h);
        fclose(fd_bmp);
        fclose(fd_til);
        return 1;
    }
    printf("INFO: width: %d height: %d\nBits per channel: %d\nChannels: %d\n", bmp_width, bmp_height, bmp_bitsperchannel, bmp_channels);

    uint8_t *bmp_image = malloc(bmp_width * bmp_height);
    bmp_err = bmpread_load_image(bmp_h, &bmp_image);

    if (bmp_err)
    {
        printf("Failed to load image: %s\n", bmp_errmsg(bmp_h));
        bmp_free(bmp_h);
        free(bmp_palette);
        free(bmp_image);
        fclose(fd_bmp);
        fclose(fd_til);
        return 1;
    }

    /*
        Convert BMP image an palette to .til
    */

#define TIL_HEADER_SIZE 8
#define TIL_MAGIC "TIL"
#define TIL_MAGIC_OFF 0                         //"til"
#define TIL_PAL_COLORS_OFF 3                    // 0-255 (total colors - 1)
#define TIL_TILCNT_OFF 4                        // 1-1024 (16 bit)
#define TIL_PAL_START 8                         // palette start
#define TIL_TIL_START TIL_PAL_START + (256 * 2) // 256 posible 16bit ARGB4444 colors

    uint8_t *til_header;
    til_header = malloc(TIL_HEADER_SIZE);

    memset(til_header, 0, TIL_HEADER_SIZE);
    memcpy(til_header + TIL_MAGIC_OFF, TIL_MAGIC, 3);

    til_header[TIL_PAL_COLORS_OFF] = (uint8_t)bmp_pal_num_colors - 1;

    uint16_t tile_cnt = ((bmp_width / TILE_SIZE) * (bmp_height / TILE_SIZE));
    memcpy(&til_header[TIL_TILCNT_OFF], &tile_cnt, 2);

    uint8_t *til_pal = malloc(256 * 4);
    // Copy and transform palete entries
    for (int i = 0; i < bmp_pal_num_colors; i++)
    {
        // input RGB0
        uint8_t bmp_r = bmp_palette[i * 4];
        uint8_t bmp_g = bmp_palette[i * 4 + 1];
        uint8_t bmp_b = bmp_palette[i * 4 + 2];

        uint16_t pal_argb;
        uint16_t *pal_dst = (uint16_t *)&til_pal[i * 2];

        pal_argb = 0xF000;
        pal_argb |= ((bmp_r & 0xF0) >> 4) << 8;
        pal_argb |= ((bmp_g & 0xF0) >> 4) << 4;
        pal_argb |= ((bmp_b & 0xF0) >> 4);

        // Color 0 always transparent
        if (i != 0)
            *pal_dst = pal_argb;
        else
            *pal_dst = 0x0000;
    }

    // Write header
    if (fwrite(til_header, 1, TIL_HEADER_SIZE, fd_til) != TIL_HEADER_SIZE)
    {
        perror("Error writing to til file");
        free(til_pal);
        free(til_header);
        bmp_free(bmp_h);
        free(bmp_image);
        free(bmp_palette);
        fclose(fd_bmp);
        fclose(fd_til);
        return 1;
    }

    // Write palette
    if (fwrite(til_pal, 2, 256, fd_til) != 256)
    {
        perror("Error writing to til file");
        free(til_pal);
        free(til_header);
        bmp_free(bmp_h);
        free(bmp_image);
        free(bmp_palette);
        fclose(fd_bmp);
        fclose(fd_til);
        return 1;
    }

    // Generate tile data from image
    uint8_t *til_data;
    til_data = malloc(bmp_width * bmp_height); // Same size as image, but diferent aragement
    int tiles_x = bmp_width / TILE_SIZE;
    int tiles_y = bmp_height / TILE_SIZE;
    uint32_t til_data_off = 0;
    uint32_t frame_in = 0;

    for (int frame_y = 0; frame_y < tiles_y; frame_y++)
    {
        for (int frame_x = 0; frame_x < tiles_x; frame_x++)
        {
            frame_in = (frame_x * TILE_SIZE) + (frame_y * bmp_width);
            for (int n = 0; n < TILE_SIZE * TILE_SIZE; n++)
            {

                /*
                if (frame_x == 0 && frame_y == 0)
                {
                    printf("DEBUG: frame_in %d, til_data_off %d n: %d\n", frame_in, til_data_off, n);
                }
                */
                til_data[til_data_off] = bmp_image[frame_in];

                frame_in++;
                if (frame_in % TILE_SIZE == 0)
                    frame_in += bmp_width - TILE_SIZE;
                til_data_off++;
            }
        }
    }

    if (fwrite(til_data, 1, bmp_width * bmp_height, fd_til) != bmp_width * bmp_height)
    {
        perror("Error writing to til file");
        free(til_data);
        free(til_pal);
        free(til_header);
        bmp_free(bmp_h);
        free(bmp_image);
        free(bmp_palette);
        fclose(fd_bmp);
        fclose(fd_til);
        return 1;
    }

    free(til_data);
    free(til_pal);
    free(til_header);
    bmp_free(bmp_h);
    free(bmp_image);
    free(bmp_palette);
    fclose(fd_bmp);
    fclose(fd_til);
    return 0;
}