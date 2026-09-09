#pragma once

#include <cstdint>
#include <ARES_Engine/math/vector2i.hpp>
#include <vdp_api.h>
// TODO: add destructor
namespace ARES_Engine
{
    class Sprite
    {
    public:
        Sprite();

        void set_position(const Vector2i &position);
        void set_scale(uint8_t scale);
        void set_flip(bool horizontal, bool vertical);
        void set_palette(uint8_t palette);
        void set_visible(bool visible);
        void update(void);

        // Will not modify the Sprite object
        int get_x() const;
        int get_y() const;

        uint8_t get_scale() const;
        uint8_t get_palette() const;

        bool get_horizontal_flip() const;
        bool get_vertical_flip() const;
        bool get_visible() const;

    private:
        static bool m_used_ids[64];
        sprite_attribute_t spr;
        int id = -1;
    };

}