// Is the owner of sprite(s), gets refrence to resource_library where it pulss clips from
#pragma once

#include <ARES_Engine/animation.hpp>
#include <ARES_Engine/sprite.hpp>
#include <ARES_Engine/engine.hpp>

#include <stdexcept>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace ARES_Engine
{
    class Animator
    {
#define ANIMATOR_MAX_USED_SPRITES 16
    public:
        Animator(int used_sprites = 1)
        {
            if (used_sprites < 0 ||
                used_sprites > ANIMATOR_MAX_USED_SPRITES)
            {
                throw std::invalid_argument(
                    "Argument used_sprites must be between 0 and " +
                    std::to_string(ANIMATOR_MAX_USED_SPRITES));
            }

            // Initialize position
            pos = Vector2i(0, 0);

            // Reserve memory for all requested sprites
            sprites.reserve(used_sprites);

            // Allocate hardware sprites
            for (int i = 0; i < used_sprites; i++)
            {
                sprites.emplace_back();
            }

            // Update hardware sprite visibility
            for (auto &sprite : sprites)
            {
                sprite.update();
            }
        }

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
        void apply();

        // State queries.
        bool is_playing() const;
        bool is_paused() const;
        bool is_finished() const;

        void set_pos(Vector2i pos)
        {
            this->pos = pos;
        }

        Vector2i get_pos()
        {
            return pos;
        }

        std::size_t get_frame_index() const;

        std::shared_ptr<const Animation_clip> get_clip() const;

    private:
        std::shared_ptr<const Animation_clip> clip;
        std::vector<Sprite> sprites;

        std::size_t current_frame = 0;
        std::size_t current_duration = 0;
        std::uint64_t elapsed_ms = 0;

        bool loop = false;
        std::size_t num_frames = 0;

        Vector2i pos = {};

        bool playing = false;
        bool paused = false;
        bool finished = false;
    };
}