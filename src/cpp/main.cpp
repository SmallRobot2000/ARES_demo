#include <app.h>
#include <vdp.h>
#include <vdp_api.h>
#include <kbd.h>

keyboard_event_t ev;
sprite_attribute_t spr;

static const uint8_t sprite32_smile[32 * 32] = {
    // row 0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    // row 1
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    // row 2
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    // row 3
    0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    // row 4
    0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0,

    // row 5
    0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0,

    // row 6
    0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0,

    // row 7
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 8
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 9
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 10 - eyes
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 11 - eyes
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 12 - eyes
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 13 - eyes
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 14
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 15
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 16
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 17
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 1, 1, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 18
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 19 - smile edges
    0, 0, 0, 0, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 20
    0, 0, 0, 0, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 21
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 22
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 23
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 1, 1, 1, 1, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 24
    0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

    // row 25
    0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0,

    // row 26
    0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0,

    // row 27
    0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0,

    // row 28
    0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    // row 29
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    // row 30
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    // row 31
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void start()
{

    vdp_b0_enable();
    vdp_s0_enable();
    vdp_b0_enable_linux_mode();

    for (int i = 0; i < 640 * 480; i++)
    {
        b0[i] = 0xFFFF;
    }

    /*
        Spr init
    */
    spr.x_pos = 16;
    spr.y_pos = 16;
    spr.offset = 0;
    spr.h_flip = 0;
    spr.v_flip = 0;
    spr.pal_num = 0;
    spr.scale = 0;
    spr.size = 1;
    spr.active = 1;

    vdp_s0_write_sprite_attribute(spr, 0);

    while (!vdp_is_v_blank())
        ;
    while (vdp_is_v_blank())
        ;
    // Start of frame
    int cnt = 0;
    while (!vdp_is_v_blank())
    {
        vdp_s0_load_sprite_data(sprite32_smile, 32 * 32, 0);
        cnt++;
    }
    printf("32x32 data sends in one frame: %d \n", cnt);

    /*
     * Test floating-point performance.
     *
     * Keep the operations dependent on the previous result so the compiler
     * cannot trivially calculate the final result in advance.
     */
    cnt = 0;

    float f = 1.234567f;
    volatile float fp_result;

    while (vdp_is_v_blank())
        ;
    while (!vdp_is_v_blank())
    {
        f = f * 1.000001f + 0.000001f;
        f = f / 1.0000001f;

        cnt++;
    }

    /*
     * Make the result observable so the compiler cannot remove the loop.
     */
    fp_result = f;

    printf("Floating-point loops in one frame: %u\n", cnt);
    printf("Floating-point result: %f\n", (double)fp_result);

    while (vdp_is_v_blank())
        ;
    /*
     * Integer benchmark.
     */
    cnt = 0;

    uint32_t ia = 0x12345678u;
    uint32_t ib = 0x87654321u;

    while (!vdp_is_v_blank())
    {
        ia = ia * 1664525u + ib;
        ib = ib / 1103515245u;

        cnt++;
    }

    printf("Integer mul/add loops in one frame: %u\n", cnt);
}

void loop()
{
    if (keyboard_get_event(&ev) == 1)
    {
        if (ev.key == KEY_D)
        {
            spr.x_pos += 1;
        }

        if (ev.key == KEY_A)
        {
            spr.x_pos -= 1;
        }

        if (ev.key == KEY_W)
        {
            spr.y_pos -= 1;
        }

        if (ev.key == KEY_S)
        {
            spr.y_pos += 1;
        }

        if (ev.key == KEY_F)
        {
            spr.scale += 1;
        }

        if (ev.key == KEY_G)
        {
            spr.scale -= 1;
        }

        vdp_s0_write_sprite_attribute(spr, 0);
    }
}