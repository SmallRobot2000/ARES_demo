#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <bmplib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int size = 0;
    if (argc != 3)
    {
        printf("Usage: %s <input_file.bmp> <output_file.spr>\n", argv[0]);
        return 1;
    }
    printf("Convert to:\n1. 16x16 sprites(default)\n2. 32x32 sprites\n");
    printf("Select(1/2/none) ");
    char usel = getchar();
    if (usel == '2')
    {
        printf("You selected 32x32\n");
        size = 32;
    }
    else
    {
        printf("You selected 16x16\n");
        size = 16;
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

    FILE *fd_spr = fopen(argv[2], "wb");
    if (fd_spr == NULL)
    {
        fclose(fd_bmp);
        perror("Failed to open/create output SPR file ");
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
        fclose(fd_spr);
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
        fclose(fd_spr);
        return 1;
    }

    if (bmp_pal_num_colors > 256)
    {
        printf("%s has too many palette colors!\nUp to 256 | Actual: %d", argv[1], bmp_pal_num_colors);
        bmp_free(bmp_h);
        fclose(fd_bmp);
        fclose(fd_spr);
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
        fclose(fd_spr);
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
        fclose(fd_spr);
        return 1;
    }
    /*
        Check info
    */
    if (bmp_width % size != 0 && bmp_width > 0)
    {
        printf("%s is incorect width for selected sprite size of %d!\n", argv[1], size);
        bmp_free(bmp_h);
        fclose(fd_bmp);
        fclose(fd_spr);
        return 1;
    }
    if (bmp_height % size != 0 && bmp_height > 0)
    {
        printf("%s is incorect height for selected sprite size of %d!\n", argv[1], size);
        bmp_free(bmp_h);
        fclose(fd_bmp);
        fclose(fd_spr);
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
        fclose(fd_spr);
        return 1;
    }

    /*
        Convert BMP image an palette to .spr
    */

#define SPR_HEADER_SIZE 32
#define SPR_MAGIC "SPR"
#define SPR_MAGIC_OFF 0                            //"SPR"
#define SPR_PAL_COLORS_OFF 3                       // 0-255 (total colors - 1)
#define SPR_SPRCNT_OFF 4                           // 0-n
#define SPR_SPRSIZE_OFF 5                          // 16 or 32
#define SPR_PAL_START 32                           // palette start
#define SPR_SPRITE_START SPR_PAL_START + (256 * 4) // 256 posible ARGB4444 colors

    uint8_t *spr_header;
    spr_header = malloc(SPR_HEADER_SIZE);

    memset(spr_header, 0, SPR_HEADER_SIZE);
    memcpy(spr_header + SPR_MAGIC_OFF, SPR_MAGIC, 3);

    spr_header[SPR_PAL_COLORS_OFF] = (uint8_t)bmp_pal_num_colors - 1;
    spr_header[SPR_SPRSIZE_OFF] = (uint8_t)size;

    uint8_t *spr_pal = malloc(256 * 4);
    // Copy and transform palete entries
    for (int i = 0; i < bmp_pal_num_colors; i++)
    {
        // input RGB0
        uint8_t bmp_r = bmp_palette[i * 4];
        uint8_t bmp_g = bmp_palette[i * 4 + 1];
        uint8_t bmp_b = bmp_palette[i * 4 + 2];

        uint16_t pal_argb;
        uint16_t *pal_dst = (uint16_t *)&spr_pal[i * 2];

        pal_argb = 0xF000;
        pal_argb |= (bmp_r & 0xF0 >> 4) << 8;
        pal_argb |= (bmp_g & 0xF0 >> 4) << 4;
        pal_argb |= (bmp_b & 0xF0 >> 4);

        //Color 0 always transparent
        if (i != 0)
            *pal_dst = pal_argb;
        else
            *pal_dst = 0x0000;
    }

    // Write header
    if (fwrite(spr_header, 1, SPR_HEADER_SIZE, fd_spr) != SPR_HEADER_SIZE)
    {
        perror("Error writing to spr file");
        free(spr_pal);
        free(spr_header);
        bmp_free(bmp_h);
        free(bmp_image);
        free(bmp_palette);
        fclose(fd_bmp);
        fclose(fd_spr);
        return 1;
    }

    // Write palette
    if (fwrite(spr_pal, 2, 256, fd_spr) != 256)
    {
        perror("Error writing to spr file");
        free(spr_pal);
        free(spr_header);
        bmp_free(bmp_h);
        free(bmp_image);
        free(bmp_palette);
        fclose(fd_bmp);
        fclose(fd_spr);
        return 1;
    }

    free(spr_pal);
    free(spr_header);
    bmp_free(bmp_h);
    free(bmp_image);
    free(bmp_palette);
    fclose(fd_bmp);
    fclose(fd_spr);
    return 0;
}