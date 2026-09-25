#pragma once
#include <ARES_Engine/sprite.hpp>
#include <ARES_Engine/animation.hpp>
#include <ARES_Engine/resource_library.hpp>
#include <vector>
#include <string>
#include <filesystem>
#include <optional>
#define MAX_HW_ID 128

/*
 * TODO:
 * optimize void load_bitmap_data - old API does loading line by line - slower bc disk slow
 */
namespace ARES_Engine::Engine
{

    /**
     * @brief Rendering layer identifiers used by the engine.
     *
     * These values correspond to the VDP rendering layers that can be
     * enabled or disabled independently.
     */
    enum Layer
    {
        Sprite, /**< Sprite layer (S0). */
        Tile0,  /**< Tile layer T0. */
        Tile1,  /**< Tile layer T1. */
        Bitmap  /**< Bitmap layer B0. */
    };

    /**
     * @brief Describes one logical sprite frame.
     *
     * A frame may use one or more hardware sprite IDs, depending on its size.
     */
    struct Frame
    {
        uint8_t size;      /** Number of hardware sprite IDs used by this frame. */
        uint8_t hw_ids[4]; /** Hardware sprite IDs assigned to this frame. */
    };

    /**
     * @brief Describes one logical tile set.
     *
     * A Tile_set may use one or more hardware tileset IDs, depending on its count.
     */
    struct Tile_set
    {
        uint16_t count;    /** Number of hardware tileset IDs used by this Tile_set. */
        uint16_t start_id; /** First tile ID */
        Layer layer;       /** Layer on whitch this tile set exists */
    };

    /**
     * @brief Load sprite graphics data from a .spr file.
     *
     * Allocates hardware sprite-data slots as required and returns a list
     * describing the loaded frames.
     *
     * @param filename Path to the .spr file.
     *
     * @return Vector containing the loaded sprite frame descriptions.
     *
     * @throws std::runtime_error if the file cannot be opened, read,
     *         validated, or if insufficient hardware sprite-data resources
     *         are available.
     */
    std::vector<Engine::Frame> load_sprite_data(const std::filesystem::path &filename);

    /**
     * @brief Load a sprite palette from a .pal file.
     *
     * Loads palette data into the VDP sprite palette memory.
     *
     * @param filename Path to the .pal file.
     *
     * @throws std::runtime_error if the file cannot be opened or read.
     */
    void load_sprite_palette(std::filesystem::path filename, Sprite::Palette pal_num);

    /**
     * @brief Load tile graphics data.
     *
     * Loads tile data into the VDP tile data memory.
     *
     * @param filename Path to the .til file.
     *
     * @param layer Layer to witch to load tile data
     *
     * @throws std::runtime_error if the file cannot be opened or read.
     *
     * @returns Tile_set structure that describes loaded tile set.
     */
    Tile_set load_tile_set(std::filesystem::path filename, Engine::Layer layer);

    /**
     * @brief Release hardware tile-data resources used by a Tile_set.
     *
     * Marks all hardware IDs referenced by the supplied Tile_set as unused.
     *
     * @param tile_set Tile_set containing tile data descriptions to release.
     */
    void free_tile_set(Tile_set tile_set);

    /**
     * @brief Load a tile map layer by numeric layer ID.
     *
     * Loads the tile-map data from the specified file and copies the selected
     * source layer into the requested engine tile layer.
     *
     * @param filename   Path to the tile-map file.
     * @param layer_id   ID of the layer to load.
     * @param tile_layer Destination engine layer.
     *
     * @throws std::runtime_error If the file cannot be opened or the requested
     *                            layer cannot be loaded.
     */
    void load_tilemap(const std::filesystem::path &filename, int layer_id, Engine::Layer tile_layer);

    /**
     * @brief Load a tile map layer by layer name.
     *
     * Loads the tile-map data from the specified file and copies the selected
     * source layer into the requested engine tile layer.
     *
     * @param filename   Path to the tile-map file.
     * @param layer_name Name of the layer to load.
     * @param tile_layer Destination engine layer.
     *
     * @throws std::runtime_error If the file cannot be opened or the requested
     *                            layer cannot be loaded.
     */
    void load_tilemap(const std::filesystem::path &filename, const char *layer_name, Engine::Layer tile_layer);

    /**
     * @brief Load bitmap graphics data into video memory.
     *
     * Loads bitmap data from the specified file and places it at the given
     * destination position.
     *
     * @param filename Path to the bitmap data file.
     * @param pos      Destination position where the bitmap data will be loaded.
     *
     * @throws std::runtime_error If the file cannot be opened or the bitmap
     *                            data cannot be loaded.
     */
    void load_bitmap_data(const std::filesystem::path &filename, Vector2i pos);

    /**
     * @brief Release hardware sprite-data resources used by a frame array.
     *
     * Marks all hardware IDs referenced by the supplied frames as unused.
     * The frame vector may also be cleared by the implementation.
     *
     * @param array Vector containing sprite frame descriptions to release.
     */
    void free_sprite_data(std::vector<Engine::Frame> &array);

    /**
     * @brief Enable a VDP rendering layer.
     *
     * @param layer Layer to enable.
     */
    void enable_layer(Engine::Layer layer);

    /**
     * @brief Disable a VDP rendering layer.
     *
     * @param layer Layer to disable.
     */
    void disable_layer(Engine::Layer layer);

    void set_layer_offset(Vector2i offset, Engine::Layer layer);

    struct Animation_clip load_animation_clip(const std::filesystem::path &filename, Resource_library &rl);

    /**
     * @brief Tracks allocation of hardware sprite-data IDs.
     *
     * Each element corresponds to one hardware sprite-data slot.
     *
     * false = slot is free
     * true  = slot is currently allocated
     *
     * Being inline allows this variable to be defined in the header without
     * causing multiple-definition linker errors in C++17 and later.
     */
    inline bool _sprite_hw_id_used[128] = {};

    inline bool _tiles0_hw_id_used[1024] = {};
    inline bool _tiles1_hw_id_used[1024] = {};

    inline std::optional<std::size_t> find_free_range(
        const bool *used,
        std::size_t size,
        std::size_t n)
    {
        if (n == 0 || n > size)
            return std::nullopt;

        std::size_t count = 0;

        for (std::size_t i = 0; i < size; ++i)
        {
            if (!used[i])
            {
                ++count;

                if (count == n)
                    return i - n + 1;
            }
            else
            {
                count = 0;
            }
        }

        return std::nullopt;
    }

    inline std::optional<std::size_t> find_free_tile_range(
        Engine::Layer layer,
        std::size_t n)
    {
        const bool *used = nullptr;

        switch (layer)
        {
        case Engine::Layer::Tile0:
            used = _tiles0_hw_id_used;
            break;

        case Engine::Layer::Tile1:
            used = _tiles1_hw_id_used;
            break;

        default:
            return std::nullopt;
        }

        return find_free_range(used, 1024, n);
    }

    inline void free_tile_range(
        Engine::Layer layer,
        std::size_t offset,
        std::size_t tile_cnt)
    {
        if (tile_cnt == 0 ||
            offset >= 1024 ||
            tile_cnt > 1024 - offset)
        {
            throw std::invalid_argument("Invalid tile range");
        }

        bool *used = nullptr;

        switch (layer)
        {
        case Engine::Layer::Tile0:
            used = _tiles0_hw_id_used;
            break;

        case Engine::Layer::Tile1:
            used = _tiles1_hw_id_used;
            break;

        default:
            throw std::invalid_argument("Invalid tile layer");
        }

        for (std::size_t i = offset; i < offset + tile_cnt; ++i)
            used[i] = false;
    }

} // namespace ARES_Engine::Engine