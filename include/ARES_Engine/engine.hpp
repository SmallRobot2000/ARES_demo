#pragma once
#include <ARES_Engine/sprite.hpp>
#include <vector>
#include <string>
#include <filesystem>

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
        uint8_t size;      /**< Number of hardware sprite IDs used by this frame. */
        uint8_t hw_ids[4]; /**< Hardware sprite IDs assigned to this frame. */
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
    std::vector<Engine::Frame> load_sprite_data(std::filesystem::path filename);

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
     */
    void load_tile_data(std::filesystem::path filename, Engine::Layer layer);

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

} // namespace ARES_Engine::Engine