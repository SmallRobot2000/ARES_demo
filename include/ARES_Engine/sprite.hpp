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

        enum class Scale
        {
            x1 = 0,
            x2 = 1,
            x4 = 2,
            x8 = 3
        };

        enum class Size
        {
            x16 = 0,
            x32 = 1
        };

        enum class Palette
        {
            Pal_0 = 0,
            Pal_1 = 1,
            Pal_2 = 2,
            Pal_3 = 3
        };

        void set_position(const Vector2i &position);
        void set_scale(Scale scale);
        void set_flip(bool horizontal, bool vertical);
        void set_palette(Palette palette);
        void set_visible(bool visible);
        void set_frame(uint8_t frame_id);
        void set_size(Size pixel_size);
        void update(void);

        // Will not modify the Sprite object
        int get_x() const;
        int get_y() const;

        Vector2i get_position() const;

        Scale get_scale() const;
        Palette get_palette() const;
        Size get_size() const;

        bool get_horizontal_flip() const;
        bool get_vertical_flip() const;
        bool get_visible() const;

        static uint8_t get_remaining_ids();

    private:
        static bool m_used_ids[64];
        sprite_attribute_t spr;
        int id = -1;
    };

}