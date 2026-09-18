#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <filesystem>

#include <ARES_Engine/engine.hpp>
namespace ARES_Engine
{
    class Tileset_asset
    {
    public:
        Tileset_asset(const std::string &name,
                      const std::filesystem::path &path,
                      const Engine::Layer tile_layer = Engine::Layer::Tile0)
        {
            if (name.empty())
                throw std::invalid_argument("Argument name can not be empty!");

            our_name = name;

            tile_set = Engine::load_tile_set(path, tile_layer);
        }

        ~Tileset_asset()
        {
            Engine::free_tile_set(tile_set);
        }

        const std::string &get_name() const
        {
            return our_name;
        }

    private:
        std::string our_name;
        Engine::Tile_set tile_set;
    };

}