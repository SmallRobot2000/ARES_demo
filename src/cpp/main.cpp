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
#include <memory>

// Math ඞ
// Color ඞ
// Bitmap ඞ
// Sprite ඞ
// Engine ඞ
// Input ඞ
// World - very later
// Animation - very very later

// CTRL + K + C to comment selected lines
// CTRL + K + U to uncomment selected lines

using namespace ARES_Engine;

Sprite *spr;

void start(int argc, char **argv)
{

    Engine::enable_layer(Engine::Bitmap);
    Engine::enable_layer(Engine::Sprite);

    auto start_point = Vector2i(100, 100);
    auto end_point = Vector2i(200, 200);
    auto color = Bitmap::Color::Red;

    Bitmap::set_rectangle(start_point, end_point, color);

    if (!Input::initialize())
        return;

    auto loaded_frames = Engine::load_sprite_data("./assets/test.spr");

    Engine::load_sprite_palette("./assets/test1.pal", Sprite::Palette::Pal_0);
    printf("Frame[0] = %d", loaded_frames[0].hw_ids[1]);

    spr = new Sprite();
    spr->set_visible(true);
    spr->set_frame(0);
    spr->set_scale(Sprite::Scale::x2);
    spr->set_palette(Sprite::Palette::Pal_0);
    spr->set_size(Sprite::Size::x32);
    spr->set_position(Vector2i(32, 32));
    spr->update();

    Engine::load_tile_set("assets/test.til", Engine::Layer::Tile0);
    Engine::load_tile_set("assets/test.til", Engine::Layer::Tile1);

    Engine::load_tilemap("assets/test.map", "Tile Layer 1", Engine::Layer::Tile0);
    Engine::load_tilemap("assets/test.map", "Tile Layer 1", Engine::Layer::Tile1);

    Engine::load_bitmap_data("assets/bitmap1.b0", Vector2i(0, 0));

    Engine::enable_layer(Engine::Layer::Tile0);
    Engine::enable_layer(Engine::Layer::Tile1);
    //  Engine::free_sprite_data(loaded_frames);

    // scener_run_file("assets/scene1.sen");
}

Vector2i pos = Vector2i(100, 100);

void loop()
{
    Input::update(true);
    if (Input::is_key_down(KEY_W))
    {
        pos.y -= 1;
    }
    if (Input::is_key_down(KEY_A))
    {
        pos.x -= 1;
    }
    if (Input::is_key_down(KEY_S))
    {
        pos.y += 1;
    }
    if (Input::is_key_down(KEY_D))
    {
        pos.x += 1;
    }

    pos.x = clamp((int)pos.x, 0, 1023);
    pos.y = clamp((int)pos.y, 0, 1023);

    auto color = Bitmap::Color::Blue;
    Bitmap::set_pixel(pos, color);

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