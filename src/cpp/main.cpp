// Temp include to know what has been made and what has not been made yet
#include <ARES_Engine/math/vector2i.hpp> //Vector2i
#include <ARES_Engine/math/vector2f.hpp> //Vector2f
#include <ARES_Engine/math/math.hpp>     //Math functions
#include <ARES_Engine/bitmap/bitmap.hpp> //Bitmap
#include <ARES_Engine/bitmap/color.hpp>  //Color
#include <ARES_Engine/input.hpp>         //Input
#include <ARES_Engine/engine.hpp>        //Engine
#include <ARES_Engine/sprite.hpp>        //Sprite
#include <app.h>

// Math ඞ
// Collision -> Math
// Color ඞ
// Bitmap almost ඞ (scroll and image loading)
// Tilemap
// Sprite ඞ - add collision on/off
// Engine ඞ
// Input ඞ
// Test
// World - very later
// Animation - very very later

// CTRL + K + C to comment selected lines
// CTRL + K + U to uncomment selected lines

ARES_Engine::Sprite *spr;

void start(int argc, char **argv)
{

    ARES_Engine::Engine::enable_layer(ARES_Engine::Engine::Bitmap);
    ARES_Engine::Engine::enable_layer(ARES_Engine::Engine::Sprite);

    auto start_point = ARES_Engine::Vector2i(100, 100);
    auto end_point = ARES_Engine::Vector2i(200, 200);
    auto color = ARES_Engine::Bitmap::Color::Red;
    ARES_Engine::Bitmap::set_rectangle(start_point, end_point, color);

    if (!ARES_Engine::Input::initialize())
        return;

    std::vector<ARES_Engine::Engine::Frame> loaded_frames = ARES_Engine::Engine::load_sprite_data("./assets/test.spr");

    ARES_Engine::Engine::load_sprite_palette("./assets/test1.pal");
    printf("Frame[0] = %d", loaded_frames[0].hw_ids[1]);

    spr = new ARES_Engine::Sprite();

    spr->set_visible(true);
    spr->set_frame(0);
    spr->set_scale(ARES_Engine::Sprite::Scale::x2);
    spr->set_palette(ARES_Engine::Sprite::Palette::Pal_0);
    spr->set_size(ARES_Engine::Sprite::Size::x32);
    spr->set_position(ARES_Engine::Vector2i(32, 32));
    spr->update();

    // ARES_Engine::Engine::free_sprite_data(loaded_frames);

    // scener_run_file("assets/scene1.sen");
}

ARES_Engine::Vector2i pos = ARES_Engine::Vector2i(100, 100);

void loop()
{
    ARES_Engine::Input::update(true);
    if (ARES_Engine::Input::is_key_down(KEY_W))
    {
        pos.y -= 1;
    }
    if (ARES_Engine::Input::is_key_down(KEY_A))
    {
        pos.x -= 1;
    }
    if (ARES_Engine::Input::is_key_down(KEY_S))
    {
        pos.y += 1;
    }
    if (ARES_Engine::Input::is_key_down(KEY_D))
    {
        pos.x += 1;
    }

    pos.x = ARES_Engine::clamp((int)pos.x, 0, 1023);
    pos.y = ARES_Engine::clamp((int)pos.y, 0, 1023);

    auto color = ARES_Engine::Bitmap::Color::Blue;
    ARES_Engine::Bitmap::set_pixel(pos, color);

    spr->set_position(pos);
    spr->update();

    while (vdp_is_v_blank())
        ;
    while (!vdp_is_v_blank())
        ;
    while (vdp_is_v_blank())
        ;
}
#pragma endregion