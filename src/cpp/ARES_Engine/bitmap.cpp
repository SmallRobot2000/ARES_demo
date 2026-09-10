#include <ARES_Engine/bitmap.hpp>
#include <cmath>
#include <stdexcept>
#include <vdp_api.h>

namespace ARES_Engine
{
#pragma region Color Implementation
    const Bitmap::Color Bitmap::Color::Transparent(0, 0, 0, 0);
    const Bitmap::Color Bitmap::Color::Black(15, 0, 0, 0);
    const Bitmap::Color Bitmap::Color::White(15, 15, 15, 15);
    const Bitmap::Color Bitmap::Color::Red(15, 15, 0, 0);
    const Bitmap::Color Bitmap::Color::Green(15, 0, 15, 0);
    const Bitmap::Color Bitmap::Color::Blue(15, 0, 0, 15);
    const Bitmap::Color Bitmap::Color::Yellow(15, 15, 15, 0);
    const Bitmap::Color Bitmap::Color::Cyan(15, 0, 15, 15);
    const Bitmap::Color Bitmap::Color::Magenta(15, 15, 0, 15);

    uint16_t Bitmap::Color::get_value() const
    {
        return static_cast<uint16_t>(
            (a << 12) |
            (r << 8) |
            (g << 4) |
            (b));
    }
#pragma endregion

    void Bitmap::set_pixel(const Vector2i &position, const Bitmap::Color &color)
    {
        if (position.x < 0 || position.x >= 1024 || position.y < 0 || position.y >= 1024)
            throw std::out_of_range("Pixel position is out of bounds.");

        uint32_t address = position.y * 1024 + position.x;
        b0[address] = color.get_value();
    }

    Bitmap::Color Bitmap::get_pixel(const Vector2i &position)
    {
        if (position.x < 0 || position.x >= 1024 || position.y < 0 || position.y >= 1024)
            throw std::out_of_range("Pixel position is out of bounds.");

        uint16_t address = position.y * 1024 + position.x;
        return Bitmap::Color(b0[address]);
    }

    void Bitmap::set_rectangle(
        const Vector2i &start_point,
        const Vector2i &end_point,
        const Bitmap::Color &color)
    {
        if (start_point.x < 0 || start_point.x >= 1024 || start_point.y < 0 || start_point.y >= 1024)
            throw std::out_of_range("Start point is out of bounds.");

        if (end_point.x < 0 || end_point.x >= 1024 || end_point.y < 0 || end_point.y >= 1024)
            throw std::out_of_range("End point is out of bounds.");

        uint32_t address = start_point.y * 1024 + start_point.x;

        const uint16_t columns = std::abs(end_point.x - start_point.x) + 1;
        const uint16_t rows = std::abs(end_point.y - start_point.y) + 1;

        for (uint16_t i = 0; i < rows; i++)
        {

            for (uint16_t j = 0; j < columns; j++)
            {
                address++;
                b0[address] = color.get_value();
            }

            address += 1024 - columns;
        }
    }

    void Bitmap::set_line(
        const Vector2i &start_point,
        const Vector2i &end_point,
        const Bitmap::Color &color)
    {
        if (start_point.x < 0 || start_point.x >= 1024 || start_point.y < 0 || start_point.y >= 1024)
            throw std::out_of_range("Start point is out of bounds.");

        if (end_point.x < 0 || end_point.x >= 1024 || end_point.y < 0 || end_point.y >= 1024)
            throw std::out_of_range("End point is out of bounds.");

        int x = start_point.x;
        int y = start_point.y;

        const int dx = std::abs(end_point.x - x);
        const int dy = std::abs(end_point.y - y);

        const int sx = (x < end_point.x) ? 1 : -1;
        const int sy = (y < end_point.y) ? 1 : -1;

        int error = dx - dy;

        const uint16_t pixel = color.get_value();

        int address = y * 1024 + x;

        while (true)
        {
            b0[address] = pixel;

            if (x == end_point.x && y == end_point.y)
                break;

            const int error2 = error << 1; // error*2

            if (error2 > -dy)
            {
                error -= dy;
                x += sx;
                address += sx;
            }

            if (error2 < dx)
            {
                error += dx;
                y += sy;
                address += sy << 10; // Multiply by 1024 to move to the next row
            }
        }
    }
}
