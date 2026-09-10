#include <ARES_Engine/engine.hpp>
#include <vdp_api.h>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <string.h>
namespace fs = std::filesystem;

namespace ARES_Engine
{

    bool Engine::_sprite_hw_id_used[128] = {};


    void enable_layer(Engine::Layer layer)
    {
        switch (layer)
        {
        case Engine::Layer::Bitmap:
            vdp_b0_enable();
            break;

        case Engine::Layer::Tile0:
            vdp_t0_enable();
            break;

        case Engine::Layer::Tile1:
            vdp_t1_enable();
            break;

        case Engine::Layer::Sprite:
            vdp_s0_enable();
            break;

        default:
            break;
        }
    }

    void disable_layer(Engine::Layer layer)
    {
        switch (layer)
        {
        case Engine::Layer::Bitmap:
            vdp_b0_disable();
            break;

        case Engine::Layer::Tile0:
            vdp_t0_disable();
            break;

        case Engine::Layer::Tile1:
            vdp_t1_disable();
            break;

        case Engine::Layer::Sprite:
            vdp_s0_disable();
            break;

        default:
            break;
        }
    }

    void free_sprite_data(std::vector<Engine::Frame> &array)
    {

        for (auto var : array)
        {

            if (var.size == 16)
            {
                Engine::_sprite_hw_id_used[var.hw_ids[0]] = false;
            }
            else
            {
                Engine::_sprite_hw_id_used[var.hw_ids[0]] = false;
                Engine::_sprite_hw_id_used[var.hw_ids[1]] = false;
                Engine::_sprite_hw_id_used[var.hw_ids[2]] = false;
                Engine::_sprite_hw_id_used[var.hw_ids[3]] = false;
            }
        }

        array.clear();
    }

    std::vector<Engine::Frame> load_sprite_data(std::filesystem::path filename)
    {

        if (!fs::exists(filename) || !fs::is_regular_file(filename))
            throw std::runtime_error("Lo, this path is erroneous; it leadeth nowhere true [This path is invalid]");

        std::ifstream file(filename, std::ios::binary | std::ios::ate);

        if (!file)
            throw std::runtime_error("The file, loath to be disturbed, would not unfold itself. [Coudnt open file]");

        uint8_t header[SPR_HEADER_SIZE];

        file.read((char *)header, SPR_HEADER_SIZE);

        if (!file)
            throw std::runtime_error("The scroll would not be read; the attempt hath failed. [Reading file failed]");

        // We dont care for palette we load it before hand

        std::vector<Engine::Frame> frames;

        Engine::Frame frame;

        uint8_t sprite_size = header[SPR_SPRSIZE_OFF];

        if (memcmp(&header[SPR_MAGIC_OFF], SPR_MAGIC, 3) != 0)
            throw std::runtime_error("Alas, the file's form is wrong. [Wrong file format]");

        frame.size = sprite_size;

        frames.push_back(frame);

        return frames;
    }
}
