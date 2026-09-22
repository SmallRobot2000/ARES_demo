#pragma once
#include <ARES_Engine/sprite.hpp>
#include <ARES_Engine/math/vector2i.hpp>

#include <cstdint>
#include <vector>
#include <string>

namespace ARES_Engine
{

    /// Appearance of one sprite part at a particular animation frame.
    /// Stores data only; does not allocate a hardware Sprite.
    struct Part_pose
    {
        // Loaded graphics-data ID passed to Sprite::set_frame(), not a live
        // sprite ID or an index into the animation's frames vector.
        std::uint8_t image_id = 0;
        Vector2i offset{}; // Relative to the owning object's position.

        Sprite::Size size = Sprite::Size::x16;
        Sprite::Scale scale = Sprite::Scale::x1;
        Sprite::Palette palette = Sprite::Palette::Pal_0;

        bool visible = true;
        bool flip_x = false;
        bool flip_y = false;
    };

    /// One timed pose of all the object's sprite parts.
    struct Animation_frame
    {
        std::uint32_t duration_ms = 100; // Must be greater than zero.

        // parts[i] controls sprite i. Keep indices consistent across clips.
        // The Animator should hide sprite slots absent from this frame.
        std::vector<Part_pose> parts;
    };

    /// Reusable animation data; each Animator maintains its own playback state.
    /// Referenced graphics and palettes must remain loaded during playback.
    struct Animation_clip
    {
        // An empty clip has no pose to apply; the Animator must handle it.
        std::vector<Animation_frame> frames;
        std::string name = "";
        bool loop = true; // Otherwise finish by holding the last frame.
    };

}
