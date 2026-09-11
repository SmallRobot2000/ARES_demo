#include <ARES_Engine/engine.hpp>
#include <vdp_api.h>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <string.h>
namespace fs = std::filesystem;

namespace ARES_Engine
{


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

    void Engine::free_sprite_data(std::vector<Engine::Frame> &array)
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

    std::vector<Engine::Frame> Engine::load_sprite_data(std::filesystem::path filename)
    {

        if (!fs::exists(filename) || !fs::is_regular_file(filename))
            throw std::runtime_error("Lo, this path is erroneous; it leadeth nowhere true [This path is invalid]");

        std::ifstream file(filename, std::ios::binary | std::ios::ate);

        if (!file)
            throw std::runtime_error("The file, loath to be disturbed, would not unfold itself. [Coudnt open file]");

        uint8_t header[SPR_HEADER_SIZE];

        file.seekg(0, std::ios::beg);

        if (!file)
            throw std::runtime_error(
                "The scroll would not be read; the attempt hath failed. [Seek failed]");

        file.read(reinterpret_cast<char *>(header), SPR_HEADER_SIZE);

        if (file.fail())
            throw std::runtime_error("The scroll would not be read; the attempt hath failed. [Reading file failed]");

        // We dont care for palette we load it before hand

        std::vector<Engine::Frame> frames;

        Engine::Frame frame;

        uint8_t sprite_size = header[SPR_SPRSIZE_OFF];
        uint16_t pal_color_cnt = (uint16_t)header[SPR_PAL_COLORS_OFF] + 1;

        if (memcmp(&header[SPR_MAGIC_OFF], SPR_MAGIC, 3) != 0)
            throw std::runtime_error("Alas, the file's form is wrong. [Wrong file format]");

        frame.size = sprite_size;

        if (sprite_size != 16 && sprite_size != 32)
            throw std::runtime_error("Alas, the file's sprite is of an ill-sized form. [File has wrong sprite size]");

        uint16_t spr_cnt = (uint16_t)header[SPR_SPRCNT_OFF] + 1;
        printf("INFO: %d sprite frames\n", spr_cnt);

        if (spr_cnt > 63)
            throw std::runtime_error("Alas, the sprites are too many in number. [Too many sprites]");

        if (pal_color_cnt > 256)
            throw std::runtime_error("Lo, the palette's breadth outstrips the stage; too many colours for so small a world. [Palette too large]");

        file.seekg(SPR_HEADER_SIZE + (pal_color_cnt * 2), file.beg); // Now it's at start of sprite data

        size_t sprite_data_size = spr_cnt * (sprite_size * sprite_size);

        uint8_t *sprite_data = (uint8_t *)malloc(sprite_data_size);

        file.read((char *)sprite_data, sprite_data_size);

        if (!file)
        {
            free(sprite_data);
            throw std::runtime_error("The file doth resist our quest; to read it hath proved vain. [Reading file failed]");
        }

        uint16_t frame16_cnt = sprite_size == 16 ? spr_cnt : spr_cnt * 4;

        int hw_id;
        for (int i = 0; i < frame16_cnt; i++)
        {
            for (int n = 0; n < MAX_HW_ID; n++)
            {
                hw_id = -1;
                if (Engine::_sprite_hw_id_used[n] == false)
                {
                    Engine::_sprite_hw_id_used[n] = true;
                    hw_id = n;
                    break;
                }
                if (hw_id == -1)
                {
                    free(sprite_data);
                    throw std::runtime_error("Fie, the sprite-memory doth overflow its bounds, and now stands empty of room. [Out of sprite data memory]");
                }

                vdp_s0_load_sprite_data(&sprite_data[i * (16 * 16)], 16 * 16, n * (16 * 16));

                if (sprite_size == 16)
                {
                    frame.hw_ids[0] = n;
                    frames.push_back(frame);
                }
                else
                {                            // 32
                    frame.hw_ids[i % 4] = n; // One of four frames inside one sprite
                    if (i % 4 == 3)          // Last frame of four frames for one sprite
                    {
                        frames.push_back(frame);
                    }
                }
            }
        }
        free(sprite_data);
        return frames;
    }
}
