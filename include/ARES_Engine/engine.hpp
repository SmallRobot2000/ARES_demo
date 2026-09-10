#pragma once

namespace ARES_Engine
{

    class Engine
    {
    public:
        Engine();
        ~Engine();

        void initialize();

        // Logic id (16px 32px)

        void load_sprite_data(); // returns id array
        void load_tile_data();
        void load_tilemap();
        void load_bitmap_data();

        void unload_sprite_data();

        void enable_layer();  // Enum for layers
        void disable_layer(); // Enum for layers

        enum Layer
        {
            Sprite,
            Tile0,
            Tile1,
            Bitmap
        };
    };
}