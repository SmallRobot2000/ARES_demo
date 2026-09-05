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