// Is the owner of sprite(s), gets refrence to resource_library where it pulss clips from
#pragma once

#include <ARES_Engine/animation.hpp>
#include <ARES_Engine/sprite.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace ARES_Engine
{
    class Animator
    {
    public:
        Animator(int used_sprites = 1);

        // Set/change the animation clip.
        void set_clip(std::shared_ptr<const Animation_clip> clip);

        // Playback control.
        void play();
        void pause();
        void resume();
        void stop();
        void restart();

        // Advance animation by elapsed milliseconds.
        void update(std::uint32_t delta_ms);

        // Apply current frame without advancing time. ?
        void apply() const;

        // State queries.
        bool is_playing() const;
        bool is_paused() const;
        bool is_finished() const;

        std::size_t get_frame_index() const;

        std::shared_ptr<const Animation_clip> get_clip() const;

    private:
        std::shared_ptr<const Animation_clip> clip;
        std::vector<Sprite> sprites;

        std::size_t current_frame = 0;
        std::uint32_t elapsed_ms = 0;

        bool playing = false;
        bool paused = false;
        bool finished = false;
    };
}