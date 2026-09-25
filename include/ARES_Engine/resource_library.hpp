#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <stdexcept>

namespace ARES_Engine
{
    class Sprite_graphics;
    struct Animation_clip;
    class Tileset_asset;
    class Tilemap_asset;
    class Bitmap_asset;

    class Resource_library
    {
    public:
        Resource_library()
        {
            if (exists == true)
                throw std::runtime_error("Object of class Resource_library alredy exists");
            exists = true;
        }
        template <typename T>
        using Asset_map =
            std::unordered_map<std::string, std::shared_ptr<const T>>;

        // A named collection of one asset type.
        template <typename T>
        class Collection
        {
        public:
            void add(
                const std::string &name,
                std::shared_ptr<const T> asset)
            {
                if (!asset)
                    throw std::invalid_argument("Asset cannot be null");

                if (!assets.emplace(name, std::move(asset)).second)
                    throw std::runtime_error("Asset already exists: " + name);
            }

            std::shared_ptr<const T> get(const std::string &name) const
            {
                auto it = assets.find(name);

                if (it == assets.end())
                    throw std::runtime_error("Asset not found: " + name);

                return it->second;
            }

            bool contains(const std::string &name) const
            {
                return assets.find(name) != assets.end();
            }

            void remove(const std::string &name)
            {
                assets.erase(name);
            }

            void clear()
            {
                assets.clear();
            }

        private:
            Asset_map<T> assets;
        };

        Collection<Sprite_graphics> sprite_graphics;
        Collection<Animation_clip> animations;
        Collection<Tileset_asset> tilesets;
        Collection<Tilemap_asset> tilemaps;
        Collection<Bitmap_asset> bitmaps;

    private:
        inline static bool exists = false;
    };
}
