#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <filesystem>
#include <vector>

#include <ARES_Engine/engine.hpp>
#include <ARES_Engine/sprite.hpp>
namespace ARES_Engine
{
    class Sprite_graphics
    {
    public:
        Sprite_graphics(const std::string &name,
                        const std::filesystem::path &spr_path,
                        const std::filesystem::path &pal_path = {},
                        const Sprite::Palette pal_num = Sprite::Palette::Pal_0)
        {
            if (name.empty())
                throw std::invalid_argument("Argument name can not be empty!");

            our_name = name;

            if (!pal_path.empty())
                Engine::load_sprite_palette(pal_path, pal_num);

            frames = Engine::load_sprite_data(spr_path);
        }

        ~Sprite_graphics()
        {
            Engine::free_sprite_data(frames); // Unload our frames
        }

        // Prevent copy
        Sprite_graphics(const Sprite_graphics &) = delete;
        Sprite_graphics &operator=(const Sprite_graphics &) = delete;

        const std::string &get_name() const
        {
            return our_name;
        }

        const std::vector<Engine::Frame> &get_frames() const
        {
            return frames;
        }

    private:
        std::vector<Engine::Frame> frames;
        std::string our_name;
    };

}