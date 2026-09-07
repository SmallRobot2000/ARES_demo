#include <vdp_api.h>

/*

    Special functions

*/

/**
 * @brief Copy character/tile pixel data into T0 tile-data memory.
 *
 * @param char_data Pointer to the source character/tile data.
 * @param size      Number of bytes to copy.
 * @param offset    Destination byte offset in T0 tile-data memory.
 */
void vdp_t0_load_char_data(const void *char_data, size_t size, uint32_t offset)
{
    const uint8_t *src = (const uint8_t *)char_data;
    volatile uint8_t *dst = t0_data + offset;
    for (size_t i = 0; i < size; i++)
        dst[i] = src[i];
}

/**
 * @brief Copy character/tile pixel data into T1 tile-data memory.
 *
 * @param char_data Pointer to the source character/tile data.
 * @param size      Number of bytes to copy.
 * @param offset    Destination byte offset in T1 tile-data memory.
 */
void vdp_t1_load_char_data(const void *char_data, size_t size, uint32_t offset)
{
    const uint8_t *src = (const uint8_t *)char_data;
    volatile uint8_t *dst = t1_data + offset;
    for (size_t i = 0; i < size; i++)
        dst[i] = src[i];
}

/**
 * @brief Copy palette color data into T0 palette memory.
 *
 * @param pal_data  Pointer to the source 16-bit ARGB4444 color data.
 * @param count     Number of 16 bit palette entries to copy
 * @param offset    Destination entry offset in T0 palette memory.
 */
void vdp_t0_load_palette_at(const uint16_t *pal_data, size_t count, uint32_t offset)
{
    volatile uint32_t *dst = &r0[VDP_PAL_T0_REG + offset];
    for (size_t i = 0; i < count; i++)
        dst[i] = pal_data[i];
}

/**
 * @brief Copy palette color data into T1 palette memory.
 *
 * @param pal_data  Pointer to the source 16-bit ARGB4444 color data.
 * @param count     Number of 16 bit palette entries to copy
 * @param offset    Destination entry offset in T1 palette memory.
 */
void vdp_t1_load_palette_at(const uint16_t *pal_data, size_t count, uint32_t offset)
{
    volatile uint32_t *dst = &r0[VDP_PAL_T1_REG + offset];
    for (size_t i = 0; i < count; i++)
        dst[i] = pal_data[i];
}

/**
 * @brief Copy palette color data into T0 palette memory.
 *
 * @param pal_data  Pointer to the source 16-bit ARGB4444 color data.
 * @param count     Number of 16 bit palette entries to copy
 */
void vdp_t0_load_palette(const uint16_t *pal_data, size_t count) { vdp_t0_load_palette_at(pal_data, count, 0); }

/**
 * @brief Copy palette color data into T1 palette memory.
 *
 * @param pal_data  Pointer to the source 16-bit ARGB4444 color data.
 * @param count     Number of 16 bit palette entries to copy
 */
void vdp_t1_load_palette(const uint16_t *pal_data, size_t count) { vdp_t1_load_palette_at(pal_data, count, 0); }

/**
 * @brief Copy color data into one of the four S0 sprite palettes.
 *
 * S0 provides 4 independent palettes (0..3), each containing
 * 256 ARGB4444 color entries. Each 16-bit color is stored in the
 * lower 16 bits of a 32-bit VDP palette word.
 *
 * The destination entry is:
 *
 *     S0 palette base + (pal_num * 256) + offset
 *
 * @param pal_data Pointer to the source 16-bit ARGB4444 color data.
 * @param count    Number of palette entries to copy.
 * @param pal_num  Destination sprite palette number (0..3).
 * @param offset   Entry offset within the selected palette (0..255).
 */
void vdp_s0_load_palette_at(const uint16_t *pal_data, size_t count, uint8_t pal_num, uint32_t offset)
{
    volatile uint32_t *dst = &r0[VDP_PAL_S0_REG + ((uint32_t)pal_num * 256u) + offset];
    for (size_t i = 0; i < count; i++)
        dst[i] = pal_data[i];
}

/**
 * @brief Copy color data into one of the four S0 sprite palettes.
 *
 * S0 provides 4 independent palettes (0..3), each containing
 * 256 ARGB4444 color entries. Each 16-bit color is stored in the
 * lower 16 bits of a 32-bit VDP palette word.
 *
 * The destination entry is:
 *
 *     S0 palette base + (pal_num * 256)
 *
 * @param pal_data Pointer to the source 16-bit ARGB4444 color data.
 * @param count    Number of palette entries to copy.
 * @param pal_num  Destination sprite palette number (0..3).
 */
void vdp_s0_load_palette(const uint16_t *pal_data, size_t count, uint8_t pal_num) { vdp_s0_load_palette_at(pal_data, count, pal_num, 0); }

/**
 * @brief Copy sprite pixel data into S0 sprite-data memory.
 *
 * Each source byte represents one sprite pixel as an 8-bit palette index.
 * The actual displayed color is obtained from the sprite palette selected
 * by the corresponding sprite attribute.
 *
 * @param spr_data Pointer to the source 8-bit sprite palette-index data.
 * @param size    Number of sprite-data bytes to copy.
 * @param offset   Destination byte offset within S0 sprite-data memory.
 */
void vdp_s0_load_sprite_data(const uint8_t *spr_data, size_t size, uint32_t offset)
{
    volatile uint8_t *dst = &s0_data[offset];
    for (size_t i = 0; i < size; i++)
        dst[i] = spr_data[i];
}

/**
 * @brief Write one sprite attribute entry into the S0 sprite attribute table.
 *
 * Converts a sprite_attribute_t structure into the 64-bit S0 hardware
 * attribute format and writes it to sprite entry @p spr_num.
 *
 * @param spr_att Sprite attribute values to encode.
 * @param spr_num Destination sprite number in the S0 attribute table (0..63).
 */
void vdp_s0_write_sprite_attribute(sprite_attribute_t spr_att, uint16_t spr_num)
{
    /*
     * Hardware attribute layout:
     *
     *   bits  9:0   X position       (0..1023)
     *   bits 15:10  Reserved
     *   bits 25:16  Y position       (0..1023)
     *   bits 35:26  Reserved
     *   bits 46:36  Sprite-data offset in 16-byte words (0..2047)
     *   bit  47      Reserved
     *   bits 49:48  Palette number   (0..3)
     *   bit  50      Horizontal flip
     *   bit  51      Vertical flip
     *   bit  52      Sprite size     (0 = 16x16, 1 = 32x32)
     *   bits 54:53  Scale           (0..3, currently unused by HDL)
     *   bits 62:55  Reserved
     *   bit  63      Active
     */

    uint64_t attr = 0;

    attr |= ((uint64_t)spr_att.x_pos & 0x03FFu) << 0;
    attr |= ((uint64_t)spr_att.y_pos & 0x03FFu) << 16;
    attr |= ((uint64_t)spr_att.offset & 0x07FFu) << 36;

    attr |= ((uint64_t)spr_att.pal_num & 0x03u) << 48;
    attr |= ((uint64_t)spr_att.h_flip & 0x01u) << 50;
    attr |= ((uint64_t)spr_att.v_flip & 0x01u) << 51;
    attr |= ((uint64_t)spr_att.size & 0x01u) << 52;
    attr |= ((uint64_t)spr_att.scale & 0x03u) << 53;

    attr |= ((uint64_t)spr_att.active & 0x01u) << 63;

    s0_att[spr_num] = attr;
}

/**
 * @brief Read one sprite attribute entry from the S0 sprite attribute table.
 *
 * Reads the 64-bit S0 hardware attribute and converts it into a
 * sprite_attribute_t structure.
 *
 * @param spr_num Source sprite number in the S0 attribute table (0..63).
 *
 * @return Decoded sprite attribute structure.
 */
sprite_attribute_t vdp_s0_read_sprite_attribute(uint16_t spr_num)
{
    /*
     * Hardware attribute layout:
     *
     *   bits  9:0   X position       (0..1023)
     *   bits 15:10  Reserved
     *   bits 25:16  Y position       (0..1023)
     *   bits 35:26  Reserved
     *   bits 46:36  Sprite-data offset in 16-byte words (0..2047)
     *   bit  47      Reserved
     *   bits 49:48  Palette number   (0..3)
     *   bit  50      Horizontal flip
     *   bit  51      Vertical flip
     *   bit  52      Sprite size     (0 = 16x16, 1 = 32x32)
     *   bits 54:53  Scale            (0..3)
     *   bits 62:55  Reserved
     *   bit  63      Active
     */

    uint64_t attr = s0_att[spr_num];

    sprite_attribute_t spr_att = {0};

    spr_att.x_pos   = (uint16_t)((attr >> 0)  & 0x03FFu);
    spr_att.y_pos   = (uint16_t)((attr >> 16) & 0x03FFu);
    spr_att.offset  = (uint16_t)((attr >> 36) & 0x07FFu);

    spr_att.pal_num = (uint8_t)((attr >> 48) & 0x03u);
    spr_att.h_flip  = (uint8_t)((attr >> 50) & 0x01u);
    spr_att.v_flip  = (uint8_t)((attr >> 51) & 0x01u);
    spr_att.size    = (uint8_t)((attr >> 52) & 0x01u);
    spr_att.scale   = (uint8_t)((attr >> 53) & 0x03u);

    spr_att.active  = (uint8_t)((attr >> 63) & 0x01u);

    return spr_att;
}


/**
 * @brief Load sprite pixel data into S0 sprite-data memory from a .spr file.
 *
 * .spr format allows for loading multiple sprite images and one palette common to all sprite images.
 *
 *
 *
 * @param filename File name of .spr file to be loaded.
 * @param offset   Destination byte offset within S0 sprite-data memory.
 * @param cnt      Number of sprite-images to load.
 * @param pal_num  Destination sprite palette number (0..3).
 * @param sprite_size Size of sprites to be loaded 0 - 16x16 or 1 - 32x32 sprites
 *
 */
int vdp_s0_load_spr_file(const char *filename, uint32_t offset, uint8_t cnt, uint8_t pal_num, uint8_t sprite_size)
{
    FILE *fd = fopen(filename, "rb");

    uint8_t *header;
    uint16_t *palette;
    uint8_t *data_chunk;

    if (fd == NULL)
    {
        perror("Failed opening .spr file");
        return -1;
    }

    header = malloc(SPR_HEADER_SIZE);

    if (fread(header, 1, SPR_HEADER_SIZE, fd) != SPR_HEADER_SIZE)
    {
        perror("Failed parsing header of .spr file");
        free(header);
        fclose(fd);
        return -1;
    }

    if (memcmp(&header[SPR_MAGIC_OFF], SPR_MAGIC, 3) != 0)
    {
        printf("File format error of %s\n", filename);
        free(header);
        fclose(fd);
        return -1;
    }

    // Magic is fine so this is a .spr file

    uint16_t pal_color_cnt = (uint16_t)header[SPR_PAL_COLORS_OFF] + 1;
    uint16_t spr_cnt = (uint16_t)header[SPR_SPRCNT_OFF] + 1;
    uint8_t size = header[SPR_SPRSIZE_OFF];

    if ((spr_cnt == 0) || (size != (sprite_size ? 32 : 16)) || (cnt > spr_cnt || cnt == 0) || (pal_num > 3))
    {
        printf("Invalid file parameters for loading %d %dx%d sprites from .spr file %s\n", cnt, (sprite_size ? 32 : 16), (sprite_size ? 32 : 16), filename);
        free(header);
        fclose(fd);
        return -1;
    }

    free(header);

    // Everything is ok, now start loading
    palette = malloc(2 * pal_color_cnt);
    if (palette == NULL)
    {
        perror("Failed to allocate memory for palette");
        fclose(fd);
        return -1;
    }

    if (fread(palette, 2, pal_color_cnt, fd) != pal_color_cnt)
    {
        perror("Failed to load palette from file");
        free(palette);
        fclose(fd);
        return -1;
    }

    vdp_s0_load_palette(palette, pal_color_cnt, pal_num);
    free(palette);

    size_t data_size = cnt * (size * size);
    if ((data_size + offset) > 32768)
    {
        printf("Cannet load %lu bytes of sprite data: out of VDP memory\n", data_size);
        fclose(fd);
        return -1;
    }
    data_chunk = malloc(data_size);
    if (data_chunk == NULL)
    {
        perror("Failed to allocate memory for spr data");
        fclose(fd);
        return -1;
    }

    if (fread(data_chunk, 1, data_size, fd) != data_size)
    {
        if (errno != 0)
            perror("Failed to load spr data");
        else
            printf("Failed to load %lu bytes of spr data: out of data\n", data_size);
        free(data_chunk);
        fclose(fd);
        return -1;
    }

    vdp_s0_load_sprite_data(data_chunk, data_size, offset);
    free(data_chunk);
    fclose(fd);
    return 0;
}

/**
 * @brief Load B0 pixel data into B0 bitmap memory from a .b0 file.
 *
 * .b0 format allows for loading custom size images less than or equal to 1024x1024 into B0 layer
 *
 *
 *
 * @param filename File name of .b0 file to be loaded.
 * @param x_off    Destination x offset within B0 layer.
 * @param y_off    Destination y offset within B0 layer.
 *
 */
int vdp_b0_load_b0_file(const char *filename, int x_off, int y_off)
{
    FILE *fd = fopen(filename, "rb");

    uint8_t *header;

    if (fd == NULL)
    {
        perror("Failed opening .b0 file");
        return -1;
    }

    header = malloc(B0_HEADER_SIZE);

    if (fread(header, 1, B0_HEADER_SIZE, fd) != B0_HEADER_SIZE)
    {
        perror("Failed loading header of .b0 file");
        free(header);
        fclose(fd);
        return -1;
    }

    if (memcmp(&header[B0_MAGIC_OFF], B0_MAGIC, 3) != 0)
    {
        printf("File format error of %s\n", filename);
        free(header);
        fclose(fd);
        return -1;
    }

    // Magic is fine so this is a .b0 file

    uint16_t width;
    uint16_t height;
    memcpy(&width, &header[B0_WIDTH_OFF], sizeof(uint16_t));
    memcpy(&height, &header[B0_HEIGHT_OFF], sizeof(uint16_t));

    free(header);
    if (x_off > 1024 || x_off < -1024 || y_off > 1024 || y_off < -1024)
    {
        printf("Argument error incorect x and/or y offset!\n");
        fclose(fd);
        return -1;
    }

    if (width > 1024 || height > 1024)
    {
        printf("File format error incorect width and/or height!\n");
        fclose(fd);
        return -1;
    }

    uint16_t file_x_start = x_off < 0 ? -x_off : 0;
    uint16_t file_y_start = y_off < 0 ? -y_off : 0;

    uint16_t b0_x_start = x_off < 0 ? 0 : x_off;
    uint16_t b0_y_start = y_off < 0 ? 0 : y_off;

    uint16_t file_x_off_screen = x_off < 0 ? -x_off : (x_off + width) > 1024 ? (x_off + width) - 1024
                                                                             : 0;

    uint16_t file_y_off_screen = y_off < 0 ? -y_off : (y_off + height) > 1024 ? (y_off + height) - 1024
                                                                             : 0;

    uint16_t x_len = width - file_x_off_screen;
    uint16_t y_len = height - file_y_off_screen;

    printf(
        "file: x=%u y=%u | b0: x=%u y=%u | size: %ux%u\nOffscreen_x: %u",
        (unsigned)file_x_start,
        (unsigned)file_y_start,
        (unsigned)b0_x_start,
        (unsigned)b0_y_start,
        (unsigned)x_len,
        (unsigned)y_len,
        (unsigned)file_x_off_screen);
    // Pointer set to first pixel
    if (((file_y_start * width) + file_x_start) * 2 != 0)
        fseek(fd, ((file_y_start * width) + file_x_start) * 2, SEEK_CUR);

    printf("seek %d\n", ((file_y_start * width) + file_x_start) * 2);

    for (int y = 0; y < y_len; y++)
    {
        if (fread((void *)&b0[((y + b0_y_start) * 1024) + b0_x_start], 2, x_len, fd) != x_len)
        {
            perror("Image file read");
            fclose(fd);
            printf("Size: %d\n", x_len);
            return -1;
        }
        // Seek to start of next line
        fseek(fd, file_x_off_screen * 2, SEEK_CUR);
    }

    fclose(fd);
    return 0;
}