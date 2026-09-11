#pragma once

#include <cstdint>
#include <ARES_Engine/math/vector2i.hpp>
#include <vdp_api.h>

namespace ARES_Engine
{
    /**
     * @brief Represents one hardware-backed VDP sprite.
     *
     * Each Sprite object owns one hardware sprite ID from the VDP S0 sprite
     * engine. The class stores the software-side sprite state and writes it
     * to the hardware through update().
     */
    class Sprite
    {
    public:
        /**
         * @brief Create a new sprite.
         *
         * The constructor allocates one free hardware sprite ID.
         */
        Sprite();

        /**
         * @brief Destroy the sprite.
         *
         * Releases the hardware sprite ID owned by this object.
         */
        ~Sprite();

        /**
         * @brief Sprite scale factor.
         *
         * Scaling is applied equally in the horizontal and vertical direction.
         */
        enum class Scale
        {
            x1 = 0, /**< 1x scale. */
            x2 = 1, /**< 2x scale. */
            x4 = 2, /**< 4x scale. */
            x8 = 3  /**< 8x scale. */
        };

        /**
         * @brief Base sprite dimensions.
         */
        enum class Size
        {
            x16 = 0, /**< 16x16 pixel sprite. */
            x32 = 1  /**< 32x32 pixel sprite. */
        };

        /**
         * @brief S0 sprite palette selection.
         *
         * The VDP provides four independent sprite palettes.
         */
        enum class Palette
        {
            Pal_0 = 0, /**< Sprite palette 0. */
            Pal_1 = 1, /**< Sprite palette 1. */
            Pal_2 = 2, /**< Sprite palette 2. */
            Pal_3 = 3  /**< Sprite palette 3. */
        };

        /**
         * @brief Set the sprite position.
         *
         * @param position New sprite position in screen/VDP coordinates.
         */
        void set_position(const Vector2i &position);

        /**
         * @brief Set the sprite scale factor.
         *
         * @param scale New scale factor.
         */
        void set_scale(Scale scale);

        /**
         * @brief Set horizontal and vertical flipping.
         *
         * @param horizontal true to flip horizontally.
         * @param vertical   true to flip vertically.
         */
        void set_flip(bool horizontal, bool vertical);

        /**
         * @brief Select the sprite palette.
         *
         * @param palette Palette to use.
         */
        void set_palette(Palette palette);

        /**
         * @brief Set sprite visibility.
         *
         * @param visible true to display the sprite, false to disable it.
         */
        void set_visible(bool visible);

        /**
         * @brief Select the sprite animation/frame data id.
         *
         * @param frame_id Frame identifier to use.
         */
        void set_frame(uint8_t frame_id);

        /**
         * @brief Set the base sprite dimensions.
         *
         * @param pixel_size Sprite size, either 16x16 or 32x32.
         */
        void set_size(Size pixel_size);

        /**
         * @brief Write the current sprite state to the VDP hardware.
         *
         * Converts the current software-side sprite state into
         * sprite_attribute_t and updates the assigned hardware sprite entry.
         */
        void update(void);

        /**
         * @brief Get the current X coordinate.
         *
         * @return Current sprite X position.
         */
        int get_x() const;

        /**
         * @brief Get the current Y coordinate.
         *
         * @return Current sprite Y position.
         */
        int get_y() const;

        /**
         * @brief Get the current sprite position.
         *
         * @return Sprite position as Vector2i.
         */
        Vector2i get_position() const;

        /**
         * @brief Get the current scale factor.
         *
         * @return Current sprite scale.
         */
        Scale get_scale() const;

        /**
         * @brief Get the currently selected palette.
         *
         * @return Current sprite palette.
         */
        Palette get_palette() const;

        /**
         * @brief Get the current base sprite size.
         *
         * @return Current sprite size.
         */
        Size get_size() const;

        /**
         * @brief Check whether horizontal flipping is enabled.
         *
         * @return true if horizontally flipped.
         */
        bool get_horizontal_flip() const;

        /**
         * @brief Check whether vertical flipping is enabled.
         *
         * @return true if vertically flipped.
         */
        bool get_vertical_flip() const;

        /**
         * @brief Check whether the sprite is visible.
         *
         * @return true if the sprite is enabled and visible.
         */
        bool get_visible() const;

        /**
         * @brief Get the number of unused hardware sprite IDs.
         *
         * @return Number of available sprite IDs in the range 0..64.
         */
        static uint8_t get_remaining_ids();

    private:
        /**
         * @brief Tracks which of the 64 hardware sprite IDs are in use.
         *
         * Shared by all Sprite instances.
         */
        static bool m_used_ids[64];

        /**
         * @brief Hardware sprite attribute structure.
         *
         * Stores the values that are eventually written to the VDP S0
         * sprite attribute table.
         */
        sprite_attribute_t spr;

        /**
         * @brief Hardware sprite ID assigned to this object.
         *
         * A value of -1 means that no hardware sprite ID is currently assigned.
         */
        int id = -1;
    };

} // namespace ARES_Engine