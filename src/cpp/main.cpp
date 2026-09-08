// Temp include to know what has been made and what has not been made yet
// Fix: cpp_include in the <...>
#include <cpp_include/ARES_Engine/math/vector2i.hpp> //Vector2i
#include <cpp_include/ARES_Engine/math/vector2f.hpp> //Vector2f
#include <cpp_include/ARES_Engine/math/math.hpp>     //Math functions

// Math ඞ
// Color - bitmap only
// Sprite
// Renderer
// Engine
// Input
// Test

// CTRL + K + C to comment selected lines
// CTRL + K + U to uncomment selected lines

#pragma region Old main code
// #include <app.h>
// #include <vdp.h>
// #include <vdp_api.h>
// #include <kbd.h>
// #include <scener.h>
// #include <array>
// #include <chrono>
// keyboard_event_t ev;
// sprite_attribute_t spr;

// static const uint8_t sprite32_smile[32 * 32] = {
//     // row 0
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

//     // row 1
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

//     // row 2
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

//     // row 3
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0,

//     // row 4
//     0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0,

//     // row 5
//     0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0,

//     // row 6
//     0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0,

//     // row 7
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 8
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 9
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 10 - eyes
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 11 - eyes
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 12 - eyes
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 13 - eyes
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 14
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 15
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 16
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 17
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 1, 1, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 18
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 19 - smile edges
//     0, 0, 0, 0, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 20
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 21
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 22
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 23
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 1, 1, 1, 1, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 24
//     0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0,

//     // row 25
//     0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0,

//     // row 26
//     0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0,

//     // row 27
//     0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0,

//     // row 28
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0,

//     // row 29
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

//     // row 30
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

//     // row 31
//     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// extern int ray_demo(int argc, char **argv);
// void start(int argc, char **argv)
// {

//     vdp_b0_enable();
//     vdp_s0_enable();
//     vdp_b0_enable_linux_mode();

//     for (int i = 0; i < 1024 * 1024; i++)
//     {
//         b0[i] = 0xF008;
//     }

//     /*
//         Spr init
//     */
//     spr.x_pos = 16;
//     spr.y_pos = 16;
//     spr.offset = 0;
//     spr.h_flip = 0;
//     spr.v_flip = 0;
//     spr.pal_num = 0;
//     spr.scale = 0;
//     spr.size = 1;
//     spr.active = 1;

//     vdp_s0_write_sprite_attribute(spr, 0);

//     while (!vdp_is_v_blank())
//         ;
//     while (vdp_is_v_blank())
//         ;
//     // Start of frame
//     int cnt = 0;
//     while (!vdp_is_v_blank())
//     {
//         vdp_s0_load_sprite_data(sprite32_smile, 32 * 32, 0);
//         cnt++;
//     }
//     printf("32x32 data sends in one frame: %d \n", cnt);

//     /*
//      * Test floating-point performance.
//      *
//      * Keep the operations dependent on the previous result so the compiler
//      * cannot trivially calculate the final result in advance.
//      */
//     cnt = 0;

//     float f = 1.234567f;
//     volatile float fp_result;

//     while (vdp_is_v_blank())
//         ;
//     while (!vdp_is_v_blank())
//     {
//         f = f * 1.000001f + 0.000001f;
//         f = f / 1.0000001f;

//         cnt++;
//     }

//     /*
//      * Make the result observable so the compiler cannot remove the loop.
//      */
//     fp_result = f;

//     printf("Floating-point loops in one frame: %u\n", cnt);
//     printf("Floating-point result: %f\n", (double)fp_result);

//     while (vdp_is_v_blank())
//         ;
//     /*
//      * Integer benchmark.
//      */
//     cnt = 0;

//     uint32_t ia = 0x12345678u;
//     uint32_t ib = 0x87654321u;

//     while (!vdp_is_v_blank())
//     {
//         ia = ia * 1664525u + ib;
//         ib = ib / 1103515245u;

//         cnt++;
//     }

//     printf("Integer mul/add loops in one frame: %u\n", cnt);

//     vdp_bg_set_color(0x0000);
//     // vdp_b0_disable();
//     printf("Loading .spr\n");
//     int err = vdp_s0_load_spr_file("assets/test.spr", 0, 1, 0, 1); // Overwrite smile
//     if (err)
//         printf("Failed to laod .spr\n");

//     vdp_b0_disable_linux_mode();
//     err = vdp_b0_load_b0_file("assets/bitmap1.b0", 200, 100);
//     // Ray demo
//     // ray_demo(argc, argv);

//     printf("Load tile data\n");
//     vdp_t0_load_til_file("assets/tiles.til", 0, 10, 1);

//     t0_map[0] = 0;

//     std::array<bool, KEY_MAX + 1> tempTestFinalFinal = {};

//     auto start = std::chrono::high_resolution_clock::now();
//     tempTestFinalFinal.fill(true);
//     auto end = std::chrono::high_resolution_clock::now();

//     auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//     printf("\n");
//     printf("Execution Time: %llu microseconds\n", (unsigned long long)duration.count());

//     scener_run_file("assets/scene.sen");
// }

// void loop()
// {

//     if (keyboard_get_event(&ev) == 1)
//     {
//         if (ev.key == KEY_D)
//         {
//             vdp_b0_set_x_offset(vdp_b0_get_x_offset() + 1);
//             spr.x_pos += 1;
//         }

//         if (ev.key == KEY_A)
//         {
//             vdp_b0_set_x_offset(vdp_b0_get_x_offset() - 1);
//             spr.x_pos -= 1;
//         }

//         if (ev.key == KEY_W)
//         {
//             vdp_b0_set_y_offset(vdp_b0_get_y_offset() + 1);
//             spr.y_pos -= 1;
//         }

//         if (ev.key == KEY_S)
//         {
//             vdp_b0_set_y_offset(vdp_b0_get_y_offset() - 1);
//             spr.y_pos += 1;
//         }

//         if (ev.key == KEY_F)
//         {
//             spr.scale += 1;
//         }

//         if (ev.key == KEY_G)
//         {
//             spr.scale -= 1;
//         }

//         vdp_s0_write_sprite_attribute(spr, 0);

//         if (vdp_b0_get_x_offset() >= 1024)
//         {
//             vdp_b0_set_x_offset(0);
//         }

//         if (vdp_b0_get_y_offset() >= 1024)
//         {
//             vdp_b0_set_y_offset(0);
//         }
//     }
// }
#pragma endregion