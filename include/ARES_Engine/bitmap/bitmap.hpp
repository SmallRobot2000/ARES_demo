#pragma once
#include <cstdint>
#include <ARES_Engine/math/math.hpp>
#include <ARES_Engine/math/vector2i.hpp>
#include <ARES_Engine/bitmap/color.hpp>

// Dont need a class since everything is static
namespace ARES_Engine::Bitmap
{
    void set_pixel(const Vector2i &position, const Color &color);
    Color get_pixel(const Vector2i &position);

    void set_rectangle(
        const Vector2i &start_point,
        const Vector2i &end_point,
        const Color &color);
    void set_line( // May god help us and the compiler
        const Vector2i &start_point,
        const Vector2i &end_point,
        const Color &color);

    // Make text object
    void set_text(); // Params: position, font size, font, text, text color, text decorations (struct)
    // Get text object
    void get_text();

    void set_scrollX(int scrollX);
    void set_scrollY(int scrollY);

    void load_image();

    int get_scrollX();
    int get_scrollY();
}