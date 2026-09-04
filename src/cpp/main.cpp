#include <app.h>
#include <vdp.h>
void start()
{
    uint64_t ctrl = 0x00;
    ctrl |= (1 << VDP_BIT_CTRL_S0_EN) | (1 << VDP_BIT_CTRL_T0_EN) | (1 << VDP_BIT_CTRL_T1_EN) | (1 << VDP_BIT_CTRL_B0_EN) | (1 << VDP_BIT_CTRL_B0_LC); //Magic
    r0[VDP_CTRL_REG] = ctrl;

    for(int i = 0; i < 640*480; i++)
    {
        b0[i] = 0xFFFF;
    }
}

void loop()
{
}