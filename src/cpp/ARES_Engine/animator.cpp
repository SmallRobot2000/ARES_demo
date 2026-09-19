#include <ARES_Engine/animator.hpp>

namespace ARES_Engine
{
    // Set/change the animation clip.
    void Animator::set_clip(std::shared_ptr<const Animation_clip> clip)
    {
        if (!clip)
            throw std::runtime_error("clip must exist!");
        this->clip = clip;

        this->current_frame = 0;
        this->current_duration = 0;
        this->elapsed_ms = 0;

        this->playing = false;
        this->paused = false;
        this->finished = false;

        bool loop = this->clip.get()->loop;
        std::size_t num_frames = this->clip.get()->frames.size();
    }

    // Playback control.
    void Animator::play()
    {
        this->current_frame = 0;
        this->current_duration = 0;
        this->elapsed_ms = 0;

        this->playing = true;
        this->paused = false;
        this->finished = false;
    }
    void Animator::pause()
    {
        this->paused = true;
    }
    void Animator::resume()
    {
        if (this->finished == false)
        {
            this->paused = false;
        }
    }
    void Animator::stop()
    {
        this->playing = false;
        this->paused = false;
        this->finished = false;
    }
    void Animator::restart()
    {
        this->current_frame = 0;
        this->current_duration = 0;
        this->elapsed_ms = 0;

        this->playing = false;
        this->paused = false;
        this->finished = false;
    }

    // Advance animation by elapsed milliseconds.
    void Animator::update(std::uint32_t delta_ms)
    {
        if (!this->playing || this->paused)
            return;

        this->elapsed_ms += delta_ms;
        if (this->elapsed_ms >= this->current_duration) // Next frame
        {
            this->elapsed_ms = 0;
            this->current_frame++;
            
            if (this->loop == true && this->current_frame == this->num_frames)
            {
                this->current_frame = 0;
                this->current_duration = this->get_clip()->frames.at(current_frame).duration_ms;
                apply();
                // Forever
                return;
            }
            else if (this->current_frame == this->num_frames)
            {
                // We done
                this->finished = true;
                this->playing = false;
                return;
            }
            else
            {
                this->current_duration = this->get_clip()->frames.at(current_frame).duration_ms;
                apply();
                // In between frame
                return;
            }
        }
    }

    // Apply current frame without advancing time. ?
    void Animator::apply()
    {
        int part_cnt = 0;
        auto clp = this->clip.get();
        part_cnt = clp->frames.at(current_frame).parts.size();

        if (part_cnt > sprites.size())
            throw std::runtime_error("More parts than available sprites!");

        for (int i = 0; i < part_cnt; i++)
        {
            Vector2i offset = clp->frames.at(current_frame).parts.at(i).offset;
            bool flip_x = clp->frames.at(current_frame).parts.at(i).flip_x;
            bool flip_y = clp->frames.at(current_frame).parts.at(i).flip_y;
            std::uint8_t id = clp->frames.at(current_frame).parts.at(i).image_id;
            ARES_Engine::Sprite::Palette pal = clp->frames.at(current_frame).parts.at(i).palette;
            ARES_Engine::Sprite::Scale scale = clp->frames.at(current_frame).parts.at(i).scale;
            ARES_Engine::Sprite::Size size = clp->frames.at(current_frame).parts.at(i).size;
            bool visible = clp->frames.at(current_frame).parts.at(i).visible;

            this->sprites.at(i).set_position(offset + this->pos);
            sprites.at(i).set_flip(flip_x, flip_y);
            sprites.at(i).set_frame(id);
            sprites.at(i).set_palette(pal);
            sprites.at(i).set_scale(scale);
            sprites.at(i).set_size(size);
            sprites.at(i).set_visible(visible);

            sprites.at(i).update();
        }
    }

    // State queries.
    bool Animator::is_playing() const
    {
        return this->playing;
    }
    bool Animator::is_paused() const
    {
        return this->paused;
    }
    bool Animator::is_finished() const
    {
        return this->finished;
    }

    std::size_t Animator::get_frame_index() const
    {
        return this->current_frame;
    }

    std::shared_ptr<const Animation_clip> Animator::get_clip() const
    {
        return this->clip;
    }

}