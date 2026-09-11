#include <ARES_Engine/bitmap/color.hpp>

namespace ARES_Engine::Bitmap
{
    const Color Color::Transparent(0, 0, 0, 0);
    const Color Color::Black(15, 0, 0, 0);
    const Color Color::White(15, 15, 15, 15);
    const Color Color::Red(15, 15, 0, 0);
    const Color Color::Green(15, 0, 15, 0);
    const Color Color::Blue(15, 0, 0, 15);
    const Color Color::Yellow(15, 15, 15, 0);
    const Color Color::Cyan(15, 0, 15, 15);
    const Color Color::Magenta(15, 15, 0, 15);

    uint16_t Color::get_value() const
    {
        return static_cast<uint16_t>(
            (a << 12) |
            (r << 8) |
            (g << 4) |
            (b));
    }
}
