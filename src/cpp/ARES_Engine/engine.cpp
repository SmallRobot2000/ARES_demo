#include <ARES_Engine/engine.hpp>
#include <custom_formats.h>
#include <vdp_api.h>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <string.h>
namespace fs = std::filesystem;

namespace ARES_Engine::Engine
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
            }
            if (hw_id == -1)
            {
                free(sprite_data);
                throw std::runtime_error("Fie, the sprite-memory doth overflow its bounds, and now stands empty of room. [Out of sprite data memory]");
            }

            vdp_s0_load_sprite_data(&sprite_data[i * (16 * 16)], 16 * 16, hw_id * (16 * 16));

            if (sprite_size == 16)
            {
                frame.hw_ids[0] = hw_id;
                frames.push_back(frame);
            }
            else
            {                                // 32
                frame.hw_ids[i % 4] = hw_id; // One of four frames inside one sprite
                if (i % 4 == 3)              // Last frame of four frames for one sprite
                {
                    frames.push_back(frame);
                }
            }
        }
        free(sprite_data);
        return frames;
    }

    void load_sprite_palette(std::filesystem::path filename, Sprite::Palette pal_num)
    {
        if (!fs::exists(filename) || !fs::is_regular_file(filename))
            throw std::runtime_error("Lo, this path is erroneous; it leadeth nowhere true [This path is invalid]");

        std::ifstream file(filename, std::ios::in);

        if (!file)
            throw std::runtime_error("The file, loath to be disturbed, would not unfold itself. [Coudnt open file]");

        std::string str;

        if (!std::getline(file, str))
            throw std::runtime_error("Alas, this scroll is no true palette; its contents mock the artist's intent. [The file is not a valid palette file]");

        if (str.compare("JASC-PAL") != 0)
            throw std::runtime_error("Alas, this scroll is no true palette; its contents mock the artist's intent. [The file is not a valid palette file]");

        if (!std::getline(file, str))
            throw std::runtime_error("Alas, this scroll is no true palette; its contents mock the artist's intent. [The file is not a valid palette file]");

        // We dont care for the version
        if (!std::getline(file, str))
            throw std::runtime_error("Alas, this scroll is no true palette; its contents mock the artist's intent. [The file is not a valid palette file]");

        int col_cnt = std::stoi(str);

        if (col_cnt < 0 || col_cnt > 256)
            throw std::runtime_error("Alas, the palette swelleth beyond its due; too many colours for the frame to hold. [Too many colors in palette]");

        // We only use palette 0
        uint16_t pal[256];
        for (int i = 0; i < col_cnt; i++)
        {
            if (!std::getline(file, str))
                throw std::runtime_error("Alas, this scroll is no true palette; its contents mock the artist's intent. [The file is not a valid palette file]");

            std::istringstream iss(str);

            int r, g, b;
            iss >> r >> g >> b;

            uint16_t argb = 0xF000;
            argb |= ((r & 0xF0) >> 4) << 8;
            argb |= ((g & 0xF0) >> 4) << 4;
            argb |= ((b & 0xF0) >> 4); // Endianes stuff

            if (i == 0)
                pal[i] = 0x0000;
            else
                pal[i] = argb;
        }

        vdp_s0_load_palette(pal, col_cnt, (uint8_t)pal_num);
    }

    Tile_set load_tile_set(std::filesystem::path filename, Engine::Layer layer)
    {

        if (layer != Engine::Layer::Tile0 && layer != Engine::Layer::Tile1)
            throw std::runtime_error("Invalid layer");

        if (!fs::exists(filename) || !fs::is_regular_file(filename))
            throw std::runtime_error("Invalid path");

        std::ifstream file(filename, std::ios::in);

        if (!file)
            throw std::runtime_error("Coudn't open file");

        file.seekg(0);
        uint8_t header[TIL_HEADER_SIZE];
        file.read(reinterpret_cast<char *>(header), TIL_HEADER_SIZE);

        if (!file)
            throw std::runtime_error("Failed reading file");

        uint16_t tile_cnt;
        memcpy(&tile_cnt, &header[TIL_TILCNT_OFF], 2);

        file.close();

        // Calculate firs available offset
        auto off = find_free_tile_range(layer, tile_cnt);

        if (!off)
            throw std::runtime_error("No free tile range available");

        int ret;
        if (layer == Engine::Layer::Tile0)
            ret = vdp_t0_load_til_file(filename.c_str(), off.value() * (8 * 8), tile_cnt, 1);
        else
            ret = vdp_t1_load_til_file(filename.c_str(), off.value() * (8 * 8), tile_cnt, 1);

        if (ret != 0)
            throw std::runtime_error("Failed loading tile data");

        // Sucsess
        Tile_set tile_set;
        tile_set.count = tile_cnt;
        tile_set.layer = layer;
        tile_set.start_id = off.value();
        return tile_set;
    }

    void free_tile_set(Tile_set tile_set)
    {
        free_tile_range(tile_set.layer, tile_set.start_id, tile_set.count);
    }
    void load_tilemap(const std::filesystem::path &filename, int layer_id, Engine::Layer tile_layer)
    {
        if (tile_layer != Engine::Layer::Tile0 && tile_layer != Engine::Layer::Tile1)
        {
            throw std::runtime_error("Invalid layer");
        }
        std::ifstream file(filename);

        // failed to open
        if (!file)
        {
            throw std::runtime_error("Failed to open tilemap: " + filename.string());
        }

        uint8_t header[MAP_HEADER_SIZE];

        file.read(reinterpret_cast<char *>(header), sizeof(header));

        if (!file)
            throw std::runtime_error("Failed to read" + filename.string());

        if (memcmp(&header[MAP_HEADER_MAGIC_OFF], MAP_HEADER_MAGIC, 3) != 0)
        {
            throw std::runtime_error("Wrong format " + filename.string());
        }

        uint8_t layer_cnt = header[MAP_HEADER_LAYER_CNT_OFF];

        uint8_t layer_table[MAP_LAYER_TABLE_SIZE];
        file.read(reinterpret_cast<char *>(layer_table), sizeof(layer_table));
        if (!file)
            throw std::runtime_error("Failed to read" + filename.string());

        bool found = false;
        int cnt = 0;
        for (int i = 0; i < layer_cnt; i++)
        {

            if (layer_table[(i * MAP_LAYER_TABLE_ENTRY_SIZE) + MAP_LAYER_TABLE_ID_OFF] == layer_id)
            {
                found = true;
                cnt = i;
                break;
            }
        }

        if (!found)
            throw std::runtime_error("Failed to find layer with ID: " + std::to_string(layer_id) + " in " + filename.string());

        // Load map
        file.seekg(MAP_LAYER_DATA_OFF + (cnt * MAP_LAYER_DATA_ENTRY_SIZE));

        uint16_t *map_buf[MAP_LAYER_DATA_ENTRY_SIZE / 2];

        file.read(reinterpret_cast<char *>(map_buf), MAP_LAYER_DATA_ENTRY_SIZE);

        if (!file)
            throw std::runtime_error("Failed to read file: " + filename.string());

        memcpy(tile_layer == Engine::Layer::Tile0 ? (void *)t0_map : (void *)t1_map, map_buf, MAP_LAYER_DATA_ENTRY_SIZE);
    }

    void load_tilemap(const std::filesystem::path &filename, const char *layer_name, Engine::Layer tile_layer)
    {
        if (tile_layer != Engine::Layer::Tile0 && tile_layer != Engine::Layer::Tile1)
        {
            throw std::runtime_error("Invalid layer");
        }
        std::ifstream file(filename);

        // failed to open
        if (!file)
        {
            throw std::runtime_error("Failed to open tilemap: " + filename.string());
        }

        uint8_t header[MAP_HEADER_SIZE];

        file.read(reinterpret_cast<char *>(header), sizeof(header));

        if (!file)
            throw std::runtime_error("Failed to read" + filename.string());

        if (memcmp(&header[MAP_HEADER_MAGIC_OFF], MAP_HEADER_MAGIC, 3) != 0)
        {
            throw std::runtime_error("Wrong format " + filename.string());
        }

        uint8_t layer_cnt = header[MAP_HEADER_LAYER_CNT_OFF];

        uint8_t layer_table[MAP_LAYER_TABLE_SIZE];
        file.read(reinterpret_cast<char *>(layer_table), sizeof(layer_table));
        if (!file)
            throw std::runtime_error("Failed to read" + filename.string());

        bool found = false;
        int cnt = 0;
        char name[MAP_MAX_NAME_LEN + 1];
        for (int i = 0; i < layer_cnt; i++)
        {
            memset(name, 0, MAP_MAX_NAME_LEN + 1);
            memcpy(name, &layer_table[(i * MAP_LAYER_TABLE_ENTRY_SIZE) + MAP_LAYER_TABLE_NAME_OFF], MAP_MAX_NAME_LEN);

            if (strcmp(name, layer_name) == 0)
            {
                found = true;
                cnt = i;
                break;
            }
        }

        std::string str_name = layer_name;
        if (!found)
            throw std::runtime_error("Failed to find layer with name: \"" + str_name + "\" in " + filename.string());

        // Load map
        file.seekg(MAP_LAYER_DATA_OFF + (cnt * MAP_LAYER_DATA_ENTRY_SIZE));

        uint16_t *map_buf[MAP_LAYER_DATA_ENTRY_SIZE / 2];

        file.read(reinterpret_cast<char *>(map_buf), MAP_LAYER_DATA_ENTRY_SIZE);

        if (!file)
            throw std::runtime_error("Failed to read file: " + filename.string());

        memcpy(tile_layer == Engine::Layer::Tile0 ? (void *)t0_map : (void *)t1_map, map_buf, MAP_LAYER_DATA_ENTRY_SIZE);
    }

    void set_layer_offset(Vector2i offset, Engine::Layer layer)
    {
        switch (layer)
        {
        case Engine::Layer::Tile0:
            vdp_t0_set_x_offset(offset.x);
            vdp_t0_set_y_offset(offset.y);
            break;

        case Engine::Layer::Tile1:
            vdp_t1_set_x_offset(offset.x);
            vdp_t1_set_y_offset(offset.y);
            break;

        case Engine::Layer::Bitmap:
            vdp_b0_set_x_offset(offset.x);
            vdp_b0_set_y_offset(offset.y);
            break;

        default:
            break;
        }
    }

    void load_bitmap_data(const std::filesystem::path &filename, Vector2i pos)
    {
        int ret = vdp_b0_load_b0_file(filename.c_str(), pos.x, pos.y);

        if (ret != 0)
            throw std::runtime_error("Failed loadingfile: " + filename.string());
    }

}
