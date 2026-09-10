#pragma once
#include <cstdint>

namespace ARES_Engine
{
    class Bitmap
    {
    public:
        Bitmap();
        ~Bitmap();

        void set_pixel(int x, int y, uint16_t color);
        void load_image();

        uint16_t get_pixel(int x, int y) const;
    };
}