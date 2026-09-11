#pragma once
#include <cstdint>
#include <string>
#include <ARES_Engine/bitmap/color.hpp>

namespace ARES_Engine::Bitmap
{
    // struct Text_decorations
    // {

    //     bool bold;
    //     bool underline;
    //     bool italic;
    //     bool secret; // Temp
    // };
    struct Text_settings
    {
        uint16_t font_size;
        int font; // Temp
        Color text_color;
        Color text_background;
        // Text_decorations text_decorations;
    };

}