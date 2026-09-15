#pragma once
#include <ARES_Engine/sprite.hpp>
#include <ARES_Engine/math/vector2i.hpp>

#include <string>
#include <vector>
#include <unordered_map>

namespace ARES_Engine
{
    struct Hitbox
    {
        Vector2i start_point;
        Vector2i end_point;

        Hitbox()
            : start_point(),
              end_point()
        {
        }

        Hitbox(const Vector2i &start_point, const Vector2i &end_point)
            : start_point(start_point),
              end_point(end_point)
        {
        }
    };

    enum class Type
    {
        Default,
        Player,
        Entity,
        Object,
        Ground,
        Area
    };

    struct Collision_filter
    {
        Type type;
        uint16_t id;
    };

    class Game_object
    {
    public:
        // Get and Set for all private variables
        Game_object();
        Game_object(Sprite sprite, Hitbox hitbox, Type type);
        Game_object(
            Sprite sprite,
            Hitbox hitbox,
            Type type,
            uint16_t id,
            std::vector<Collision_filter> collision_filter,
            bool active,
            std::string name,
            std::unordered_map<std::string, std::string> details);

        // Movement
        void move(Vector2i direction_amount);
        void place(Vector2i position);

        // DEBUG
        void draw_hitbox();          // on bitmap layer
        void draw_line(Vector2i to); // From top left (on bitmap layer)
        Vector2i get_center() const; // Hitbox center

        // Collision
        std::vector<Game_object *> get_collision(bool use_filter); // Return a list of all colliding objects

        // Sprite
        Sprite &get_sprite();
        const Sprite &get_sprite() const;
        void set_sprite(Sprite sprite);

        // Hitbox
        Hitbox &get_hitbox();
        const Hitbox &get_hitbox() const;
        void set_hitbox(Hitbox hitbox);

        // Type
        Type get_type() const;
        void set_type(Type type);

        // ID
        uint16_t get_id() const;
        void set_id(uint16_t id);

        // Collision filter
        std::vector<Collision_filter> &get_collision_filter();
        const std::vector<Collision_filter> &get_collision_filter() const;
        void set_collision_filter(
            std::vector<Collision_filter> collision_filter);

        // Active
        bool get_active() const;
        void set_active(bool active);

        // Name
        std::string &get_name();
        const std::string &get_name() const;
        void set_name(std::string name);

        // Details
        std::unordered_map<std::string, std::string> &get_details();
        const std::unordered_map<std::string, std::string> &get_details() const;
        void set_details(
            std::unordered_map<std::string, std::string> details);

    private:
        Sprite sprite;
        Hitbox hitbox;
        Type type;
        uint16_t id;
        std::vector<Collision_filter> collision_filter;
        bool active;
        std::string name;
        std::unordered_map<std::string, std::string> details;
    };
}