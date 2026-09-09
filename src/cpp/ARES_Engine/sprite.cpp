#include <ARES_Engine/sprite.hpp>

namespace ARES_Engine
{
    bool Sprite::m_used_ids[64] = {};

    void Sprite::set_position(int x, int y) // USE VEC2
    {
        m_x = x;
        m_y = y;
    }
    void set_scale(uint8_t scale);
    void set_flip(bool horizontal, bool vertical);
    void set_palette(uint8_t palette);
    void set_visible(bool visible);

    bool Sprite::horizontal_flip() const
    {
        return m_h_flip;
    }
}