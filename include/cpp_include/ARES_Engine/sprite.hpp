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
        int x() const;
        int y() const;

        uint8_t scale() const;
        uint8_t palette() const;

        bool horizontal_flip() const;
        bool vertical_flip() const;
        bool visible() const;

    private:
        // int *m_x;
        // int *m_y;

        // uint8_t *m_scale;
        // uint8_t *m_palette;

        // bool *m_h_flip;
        // bool *m_v_flip;

        // bool *m_visible;

        static bool m_used_ids[64];
        sprite_attribute_t spr;
        int id = -1;
    };

}