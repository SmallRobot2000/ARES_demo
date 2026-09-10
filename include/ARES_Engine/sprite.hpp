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
        ~Sprite();

        void set_position(const Vector2i &position);
        void set_scale(uint8_t scale);
        void set_flip(bool horizontal, bool vertical);
        void set_palette(uint8_t palette);
        void set_visible(bool visible);
        void set_frame(uint8_t frame_id);
        void set_size(uint8_t pixel_size);
        void update(void);

        // Will not modify the Sprite object
        int get_x() const;
        int get_y() const;

        Vector2i get_position() const;

        uint8_t get_scale() const;
        uint8_t get_palette() const;

        bool get_horizontal_flip() const;
        bool get_vertical_flip() const;
        bool get_visible() const;
        uint8_t get_size() const;

        static uint8_t get_remaining_ids();

    private:
        static bool m_used_ids[64];
        sprite_attribute_t spr;
        int id = -1;
    };

}