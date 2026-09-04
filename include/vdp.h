#ifndef ARES_VDP_H
#define ARES_VDP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define VDP_CTRL_REG 0x00
#define VDP_STAT_REG 0x01
#define VDP_LINE_REG 0x02

#define VDP_T0_X_OFF_REG 0x04
#define VDP_T0_Y_OFF_REG 0x05

#define VDP_T1_X_OFF_REG 0x08
#define VDP_T1_Y_OFF_REG 0x09

#define VDP_BIT_CTRL_T0_EN 0x00
#define VDP_BIT_CTRL_T1_EN 0x01
#define VDP_BIT_CTRL_S0_EN 0x02
#define VDP_BIT_CTRL_B0_EN 0x03
#define VDP_BIT_CTRL_B0_LC 0x04 // Linux compatable mod e.g. no x/y offset and bitmap is raw 640x480x16bpp RGB565 with auto truncation
                                // Normal mode bitmap is 1024x1024 with x and y offset and ARGB4444

#define VDP_BIT_STAT_H_BLK 0x00
#define VDP_BIT_STAT_V_BLK 0x01

    extern volatile uint32_t *r0;
    extern volatile uint16_t *b0;

    extern volatile uint16_t *t0_map;
    extern volatile uint8_t *t0_data;

    extern volatile uint16_t *t1_map;
    extern volatile uint8_t *t1_data;

    extern volatile uint64_t *s0_att;
    extern volatile uint8_t *s0_data;

    int vdp_init(void);
    void vdp_close(void);

#ifdef __cplusplus
}
#endif

#endif /* ARES_VDP_H */
