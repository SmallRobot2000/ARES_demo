#include <ARES_Engine/sprite.hpp>
#include <stdexcept>

namespace ARES_Engine
{
    bool Sprite::m_used_ids[64] = {};

    Sprite::Sprite()
    {
        for (int i = 0; i < (int)(sizeof(m_used_ids) / sizeof(bool)); i++)
        {
            if (!m_used_ids[i]) // If this hardware sprite isn't used, occupy it
            {

                m_used_ids[i] = true;
                this->id = i; // Save the ID that we occupied
                break;
            }
        }

        if (this->id == -1) // No available sprite IDs
            throw std::runtime_error("No available sprite IDs");

        this->spr.active = 0;
        this->spr.h_flip = 0;
        this->spr.v_flip = 0;
        this->spr.x_pos = 0;
        this->spr.y_pos = 0;
        this->spr.offset = 0;
        this->spr.pal_num = 0;
        this->spr.scale = 0;
        this->spr.size = 0;

        vdp_s0_write_sprite_attribute(this->spr, (uint16_t)this->id);
    }

    void Sprite::set_position(const Vector2i &position)
    {
        this->spr.x_pos = position.x;
        this->spr.y_pos = position.y;
    }

    void Sprite::update()
    {
        vdp_s0_write_sprite_attribute(this->spr, (uint16_t)this->id);
    }

    void Sprite::set_scale(uint8_t scale) // Change to enum 1x 2x 4x 8x
    {
        this->spr.scale = scale;
    }
    void Sprite::set_flip(bool horizontal, bool vertical)
    {
        this->spr.h_flip = horizontal ? 1 : 0;
        this->spr.v_flip = vertical ? 1 : 0;
    }
    void Sprite::set_palette(uint8_t palette) // Change to enum 0-3
    {
        this->spr.pal_num = palette;
    }
    void Sprite::set_visible(bool visible)
    {
        this->spr.active = visible ? 1 : 0;
    }

    bool Sprite::get_horizontal_flip() const
    {
        return this->spr.h_flip == 0 ? false : true;
    }
}