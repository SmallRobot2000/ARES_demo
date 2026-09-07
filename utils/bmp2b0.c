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
        printf("Usage: %s <input_file.bmp> <output_file.b0>\n", argv[0]);
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

    FILE *fd_b0 = fopen(argv[2], "wb");
    if (fd_b0 == NULL)
    {
        fclose(fd_bmp);
        perror("Failed to open/create output B0 file ");
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
        fclose(fd_b0);
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
        fclose(fd_b0);
        return 1;
    }
    /*
        Check info
    */
    if (bmp_width > 1024 || bmp_width > 1024)
    {
        printf("%s is too big for b0 layer %d!\n", argv[1], size);
        bmp_free(bmp_h);
        fclose(fd_bmp);
        fclose(fd_b0);
        return 1;
    }

    printf("INFO: width: %d height: %d\nBits per channel: %d\nChannels: %d\n", bmp_width, bmp_height, bmp_bitsperchannel, bmp_channels);

    if (bmp_bitsperchannel != 8)
    {
        printf("%s has wrong bits per channel, must be 8 actual: %d\n", argv[1], bmp_bitsperchannel);
        bmp_free(bmp_h);
        fclose(fd_bmp);
        fclose(fd_b0);
        return 1;
    }
    uint8_t *bmp_image = NULL;
    bmp_err = bmpread_load_image(bmp_h, &bmp_image);

    if (bmp_err)
    {
        printf("Failed to load image: %s\n", bmp_errmsg(bmp_h));
        bmp_free(bmp_h);
        free(bmp_image);
        fclose(fd_bmp);
        fclose(fd_b0);
        return 1;
    }

    /*
        Convert BMP image to b0 image
    */

#define B0_HEADER_SIZE 8
#define B0_MAGIC "B0L"
#define B0_MAGIC_OFF 0 //"B0L"
#define B0_WIDTH_OFF 4
#define B0_HEIGHT_OFF 6
#define B0_IMAGE_OFF 8

    uint8_t *b0_header;
    b0_header = malloc(B0_HEADER_SIZE);

    memset(b0_header, 0, B0_HEADER_SIZE);
    memcpy(b0_header + B0_MAGIC_OFF, B0_MAGIC, 3);
    memcpy(&b0_header[B0_WIDTH_OFF], &bmp_width, sizeof(uint16_t));
    memcpy(&b0_header[B0_HEIGHT_OFF], &bmp_height, sizeof(uint16_t));

        // Write header
    if (fwrite(b0_header, 1, B0_HEADER_SIZE, fd_b0) != B0_HEADER_SIZE)
    {
        perror("Error writing to spr file");
        free(b0_header);
        bmp_free(bmp_h);
        free(bmp_image);
        fclose(fd_bmp);
        fclose(fd_b0);
        return 1;
    }

    uint16_t *b0_image = malloc(bmp_width * bmp_height * 2);
    // Copy and transform image pixels
    for (int i = 0; i < (bmp_width * bmp_height); i++)
    {

        uint8_t bmp_r = bmp_image[i * bmp_channels];
        uint8_t bmp_g = bmp_image[i * bmp_channels + 1];
        uint8_t bmp_b = bmp_image[i * bmp_channels + 2];

        uint16_t b0_argb;

        b0_argb = 0xF000; //Endianess stuff
        b0_argb |= ((bmp_r & 0xF0) >> 4) << 8;
        b0_argb |= ((bmp_g & 0xF0) >> 4) << 4;
        b0_argb |= ((bmp_b & 0xF0) >> 4);

        b0_image[i] = b0_argb;
    }

    // Write b0 image
    if (fwrite(b0_image, 2, bmp_width * bmp_height, fd_b0) != bmp_width * bmp_height)
    {
        perror("Error writing to b0 file");
        free(b0_image);
        free(b0_header);
        bmp_free(bmp_h);
        free(bmp_image);
        fclose(fd_bmp);
        fclose(fd_b0);
        return 1;
    }

    free(b0_header);
    bmp_free(bmp_h);
    free(bmp_image);
    free(b0_image);
    fclose(fd_bmp);
    fclose(fd_b0);
    return 0;
}