#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <kbd.h>
#include <vdp.h>
// Bok

extern void test();
int main(void)
{
    keyboard_event_t ev;

    if (vdp_init() != 0)
    {
        fprintf(stderr,
                "ERROR: UIO device 'vdp' not found\n");
        return 1;
    }

    if (keyboard_init() != 0)
    {
        fprintf(stderr,
                "ERROR: EVENT keyboard not found\n");
        vdp_close();
        return 1;
    }

    test();
    while (1)
    {

        if (keyboard_get_event(&ev) == 1)
        {

            printf("key=%u  ", ev.key);

            if (ev.value == 1)
                printf("DOWN\n");

            else if (ev.value == 0)
                printf("UP\n");

            else if (ev.value == 2)
                printf("REPEAT\n");

            if (ev.key == KEY_W)
            {
                for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
                {
                    b0[i] = 0x0000;
                }
            }

            if (ev.key == KEY_S)
            {
                for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
                {
                    b0[i] = 0xFFFF;
                }
            }

            /*
                wire [9:0]  x_pos    = spr_att_vdp_dout[9:0];   //10 bit
                wire [9:0]  y_pos    = spr_att_vdp_dout[25:16]; //10 bit
                wire [10:0] offset   = spr_att_vdp_dout[46:36]; //11 bit
                wire [1:0]  scale    = spr_att_vdp_dout[54:53]; //2 bit  ne postoji
                wire        size     = spr_att_vdp_dout[52];    //1 bit
                wire        v_flip   = spr_att_vdp_dout[51];    //1 bit
                wire        h_flip   = spr_att_vdp_dout[50];    //1 bit
                wire [1:0]  pall_num = spr_att_vdp_dout[49:48]; //2 bit
                wire        active   = spr_att_vdp_dout[63];    //1 bit

                s0_att

                s0_data

                uint64_t;




            */
            if (ev.key == KEY_D)
            {

                s0_att[0] += 1;
            }

            if (ev.key == KEY_A)
            {

                s0_att[0] -= 1;
            }
        }

        /*
         * Do other VDP/game work here.
         */
    }

    return 0;
}