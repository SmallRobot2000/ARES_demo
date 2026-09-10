#pragma once
#include <cstdint>
#include <ARES_Engine/math/math.hpp>
#include <ARES_Engine/math/vector2i.hpp>

namespace ARES_Engine
{
    class Bitmap
    {
    public:
        struct Color
        {
            uint8_t a; // 0-15
            uint8_t r; // 0-15
            uint8_t g; // 0-15
            uint8_t b; // 0-15

            Color(
                uint8_t alpha,
                uint8_t red,
                uint8_t green,
                uint8_t blue)
            {
                a = clamp(alpha, 0, 15);
                r = clamp(red, 0, 15);
                g = clamp(green, 0, 15);
                b = clamp(blue, 0, 15);
            }

            Color(uint16_t value)
            {
                a = (value >> 12) & 0xF;
                r = (value >> 8) & 0xF;
                g = (value >> 4) & 0xF;
                b = value & 0xF;
            }

            Color() : r(0), g(0), b(0), a(0) {}

            uint16_t get_value() const;

            static const Color Transparent;
            static const Color Black;
            static const Color White;
            static const Color Red;
            static const Color Green;
            static const Color Blue;
            static const Color Yellow;
            static const Color Cyan;
            static const Color Magenta;
            static const Color Gray;
        };

        static void set_pixel(const Vector2i &position, const Color &color);
        static Color get_pixel(const Vector2i &position);

        static void set_rectangle(
            const Vector2i &start_point,
            const Vector2i &end_point,
            const Color &color);
        static void set_line( // May god help us and the compiler
            const Vector2i &start_point,
            const Vector2i &end_point,
            const Color &color);

        static void set_scrollX(int scrollX);
        static void set_scrollY(int scrollY);

        static void load_image();

        static int get_scrollX();
        static int get_scrollY();
    };
}