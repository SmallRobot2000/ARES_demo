#pragma once

#include <cstdint>

// TODO: add destructor

class Sprite
{
public:
    Sprite() = default;

    void set_position(int x, int y); // USE VEC2
    void set_scale(uint8_t scale);
    void set_flip(bool horizontal, bool vertical);
    void set_palette(uint8_t palette);
    void set_visible(bool visible);

    // Will not modify the Sprite object
    int x() const;
    int y() const;

    uint8_t scale() const;
    uint8_t palette() const;

    bool horizontal_flip() const;
    bool vertical_flip() const;
    bool visible() const;

private:
    int m_x = 0;
    int m_y = 0;

    uint8_t m_scale = 0;
    uint8_t m_palette = 0;

    bool m_h_flip = false;
    bool m_v_flip = false;

    bool m_visible = true;
};