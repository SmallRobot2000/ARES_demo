#ifndef ARES_VDP_API_H
#define ARES_VDP_API_H

#include <stdint.h>
#include <stddef.h>
#include <vdp.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief S0 sprite attribute description.
     *
     * Describes one sprite entry in the S0 sprite attribute table.
     * The structure is converted to the 64-bit hardware attribute format
     * before being written to S0 attribute memory.
     *
     * Position values use the VDP's 10-bit coordinate range (0..1023).
     *
     * The data offset is expressed in 16-byte (128-bit) sprite-data words,
     * not in bytes.
     */
    typedef struct
    {
        uint16_t x_pos; /**< Sprite X position, 0..1023. */
        uint16_t y_pos; /**< Sprite Y position, 0..1023. */

        uint16_t offset; /**< Sprite-data offset in 16-byte words, 0..2047. */

        uint8_t size;   /**< Sprite size: 0 = 16x16, 1 = 32x32. */
        uint8_t h_flip; /**< Horizontal flip: 0 = normal, 1 = flipped. */
        uint8_t v_flip; /**< Vertical flip: 0 = normal, 1 = flipped. */

        uint8_t pal_num; /**< Sprite palette number, 0..3. */

        uint8_t active; /**< Sprite enabled: 0 = inactive, 1 = active. */

        uint8_t scale; /**< Scale field, 0..3; currently unused by VDP HDL. */

    } sprite_attribute_t;

    /*
        Enable functions
    */

    static inline void vdp_t0_enable(void) { r0[VDP_CTRL_REG] |= (1u << VDP_BIT_CTRL_T0_EN); }
    static inline void vdp_t1_enable(void) { r0[VDP_CTRL_REG] |= (1u << VDP_BIT_CTRL_T1_EN); }
    static inline void vdp_b0_enable(void) { r0[VDP_CTRL_REG] |= (1u << VDP_BIT_CTRL_B0_EN); }
    static inline void vdp_s0_enable(void) { r0[VDP_CTRL_REG] |= (1u << VDP_BIT_CTRL_S0_EN); }
    static inline void vdp_b0_enable_linux_mode(void) { r0[VDP_CTRL_REG] |= (1u << VDP_BIT_CTRL_B0_LC); }

    /*
        Disable functions
    */

    static inline void vdp_t0_disable(void) { r0[VDP_CTRL_REG] &= ~(1u << VDP_BIT_CTRL_T0_EN); }
    static inline void vdp_t1_disable(void) { r0[VDP_CTRL_REG] &= ~(1u << VDP_BIT_CTRL_T1_EN); }
    static inline void vdp_b0_disable(void) { r0[VDP_CTRL_REG] &= ~(1u << VDP_BIT_CTRL_B0_EN); }
    static inline void vdp_s0_disable(void) { r0[VDP_CTRL_REG] &= ~(1u << VDP_BIT_CTRL_S0_EN); }
    static inline void vdp_b0_disable_linux_mode(void) { r0[VDP_CTRL_REG] &= ~(1u << VDP_BIT_CTRL_B0_LC); }

    /*

        Bit test functions

    */

    static inline int vdp_is_v_blank(void) { return (r0[VDP_STAT_REG] & (1u << VDP_BIT_STAT_V_BLK)) != 0; }
    static inline int vdp_is_h_blank(void) { return (r0[VDP_STAT_REG] & (1u << VDP_BIT_STAT_H_BLK)) != 0; }

    /*

        Get value functions

    */

    static inline uint32_t vdp_get_current_line(void) { return r0[VDP_LINE_REG]; }

    /*

        Set value functions

    */

    static inline void vdp_t0_set_x_offset(uint32_t offset) { r0[VDP_T0_X_OFF_REG] = offset; }
    static inline void vdp_t0_set_y_offset(uint32_t offset) { r0[VDP_T0_Y_OFF_REG] = offset; }
    static inline void vdp_t1_set_x_offset(uint32_t offset) { r0[VDP_T1_X_OFF_REG] = offset; }
    static inline void vdp_t1_set_y_offset(uint32_t offset) { r0[VDP_T1_Y_OFF_REG] = offset; }
    static inline void vdp_b0_set_x_offset(uint32_t offset) { r0[VDP_B0_X_OFF_REG] = offset; }
    static inline void vdp_b0_set_y_offset(uint32_t offset) { r0[VDP_B0_Y_OFF_REG] = offset; }

    /*
        Set the background color in RGB444 format
    */
    static inline void vdp_background_color_set(uint32_t color) { r0[VDP_BG_REG] = color; }

    void vdp_t0_load_char_data(const void *char_data, size_t size, uint32_t offset);
    void vdp_t1_load_char_data(const void *char_data, size_t size, uint32_t offset);
    void vdp_t0_load_palette_at(const uint16_t *pal_data, size_t count, uint32_t offset);
    void vdp_t1_load_palette_at(const uint16_t *pal_data, size_t count, uint32_t offset);
    void vdp_t0_load_palette(const uint16_t *pal_data, size_t count);
    void vdp_t1_load_palette(const uint16_t *pal_data, size_t count);
    void vdp_s0_load_palette_at(const uint16_t *pal_data, size_t count, uint8_t pal_num, uint32_t offset);
    void vdp_s0_load_palette(const uint16_t *pal_data, size_t count, uint8_t pal_num);
    void vdp_s0_load_sprite_data(const uint8_t *spr_data, size_t count, uint32_t offset);
    void vdp_s0_write_sprite_attribute(sprite_attribute_t spr_att, uint16_t spr_num);
#ifdef __cplusplus
}
#endif

#endif /* ARES_VDP_API_H */
