#pragma once
#include <vector>
#include <string>
#include <filesystem>

#define MAX_HW_ID 128

namespace ARES_Engine::Engine
{
    // TODO: add a 1 in 1000000 chance of throw error Deez nuts

    enum Layer
    {
        Sprite,
        Tile0,
        Tile1,
        Bitmap
    };

    struct Frame
    {
        uint8_t size;
        uint8_t hw_ids[4];
    };

    void initialize();

    // Logic id (16px 32px)

    std::vector<Engine::Frame> load_sprite_data(std::filesystem::path filename); // returns frame array
    void load_tile_data();
    void load_tilemap();
    void load_bitmap_data();

    void free_sprite_data(std::vector<Engine::Frame> &array);

    void unload_sprite_data();

    void enable_layer(Engine::Layer layer);
    void disable_layer(Engine::Layer layer);

    inline bool _sprite_hw_id_used[128] = {};

}