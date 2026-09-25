// Temp include to know what has been made and what has not been made yet
#include <ARES_Engine/math/vector2i.hpp>    //Vector2i
#include <ARES_Engine/math/vector2f.hpp>    //Vector2f
#include <ARES_Engine/math/math.hpp>        //Math functions
#include <ARES_Engine/bitmap/bitmap.hpp>    //Bitmap
#include <ARES_Engine/bitmap/color.hpp>     //Color
#include <ARES_Engine/input.hpp>            //Input
#include <ARES_Engine/engine.hpp>           //Engine
#include <ARES_Engine/sprite.hpp>           //Sprite
#include <ARES_Engine/animation.hpp>        //Clip
#include <ARES_Engine/animator.hpp>         //Anim
#include <ARES_Engine/resource_library.hpp> //Resources
#include <ARES_Engine/sprite_graphics.hpp>  //Sprite assets

#include <app.h>
#include <memory>
#include <chrono>
#include <cstdint>

inline std::uint64_t get_ticks_ms()
{
    using namespace std::chrono;

    return duration_cast<milliseconds>(
               steady_clock::now().time_since_epoch())
        .count();
}

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
Sprite *spr_1;
Animator *anim_0;
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

    Engine::load_tile_set("assets/tiles/til/test.til", Engine::Layer::Tile0);
    Engine::load_tile_set("assets/tiles/til/test.til", Engine::Layer::Tile1);

    Engine::load_tilemap("assets/map/test.map", "Tile Layer 1", Engine::Layer::Tile0);
    Engine::load_tilemap("assets/map/test.map", "Tile Layer 1", Engine::Layer::Tile1);

    Engine::load_bitmap_data("assets/b0/bitmap1.b0", Vector2i(0, 0));

    Resource_library rl;
    // rl.sprite_graphics.add("player", )
    auto spr_grp = std::make_shared<Sprite_graphics>("player", "assets/test/player.spr", "assets/test/player.pal");
    rl.sprite_graphics.add("player", spr_grp);

    auto clip_0 = std::make_shared<Animation_clip>(Engine::load_animation_clip("assets/animations/player_walk.anim", rl));
    rl.animations.add("player_walk", clip_0);

    anim_0 = new Animator(2);
    anim_0->set_clip(rl.animations.get("player_walk"));
    anim_0->set_pos(Vector2i(100, 100));
    anim_0->play();

    Engine::enable_layer(Engine::Layer::Tile0);
    Engine::enable_layer(Engine::Layer::Tile1);
    Engine::enable_layer(Engine::Layer::Sprite);
    // while(1);
    //   Engine::free_sprite_data(loaded_frames);

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

    anim_0->set_pos(pos);
    anim_0->update(16);
    anim_0->apply();

    auto color = Bitmap::Color::Blue;
    Bitmap::set_pixel(pos, color);

    while (vdp_is_v_blank())
        ;
    while (!vdp_is_v_blank())
        ;
    while (vdp_is_v_blank())
        ;
}
#pragma endregion