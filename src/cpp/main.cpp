#include <ARES_Engine/math/vector2i.hpp>
#include <ARES_Engine/math/vector2f.hpp>
#include <ARES_Engine/math/math.hpp>
#include <ARES_Engine/bitmap/bitmap.hpp>
#include <ARES_Engine/bitmap/color.hpp>
#include <ARES_Engine/input.hpp>
#include <ARES_Engine/engine.hpp>
#include <ARES_Engine/sprite.hpp>

#include <linux/input.h>

#include <vdp_api.h>
#include <app.h>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <filesystem>
#include <cstdarg>
#include <chrono>

using namespace ARES_Engine;

namespace
{
// -------------------------------------------------------------------------
// DEBUGGING
// -------------------------------------------------------------------------

// 0 = errors only, 1 = important events, 2 = normal debug, 3 = verbose.
#ifndef ARES_DEMO_DEBUG_LEVEL
#define ARES_DEMO_DEBUG_LEVEL 3
#endif

    constexpr int DEBUG_FRAME_INTERVAL = 60;

    uint64_t debug_frame = 0;
    uint64_t debug_input_events = 0;
    std::chrono::steady_clock::time_point debug_start_time;

    void debug_log(const char *level, const char *format, ...)
    {
        std::printf("[ARES][%s] ", level);

        va_list args;
        va_start(args, format);
        std::vprintf(format, args);
        va_end(args);

        std::printf("\n");
        std::fflush(stdout);
    }

#define DEBUG_ERROR(...)                     \
    do                                       \
    {                                        \
        if (ARES_DEMO_DEBUG_LEVEL >= 0)      \
            debug_log("ERROR", __VA_ARGS__); \
    } while (0)
#define DEBUG_INFO(...)                     \
    do                                      \
    {                                       \
        if (ARES_DEMO_DEBUG_LEVEL >= 1)     \
            debug_log("INFO", __VA_ARGS__); \
    } while (0)
#define DEBUG_LOG(...)                       \
    do                                       \
    {                                        \
        if (ARES_DEMO_DEBUG_LEVEL >= 2)      \
            debug_log("DEBUG", __VA_ARGS__); \
    } while (0)
#define DEBUG_VERBOSE(...)                     \
    do                                         \
    {                                          \
        if (ARES_DEMO_DEBUG_LEVEL >= 3)        \
            debug_log("VERBOSE", __VA_ARGS__); \
    } while (0)

    // -------------------------------------------------------------------------
    // CONFIG
    // -------------------------------------------------------------------------

    constexpr float PLAYER_SPEED = 1.75f;

    constexpr int CENTER_X = SCREEN_WIDTH / 2;
    constexpr int CENTER_Y = SCREEN_HEIGHT / 2;

    // -------------------------------------------------------------------------
    // COLORS
    // -------------------------------------------------------------------------

    Bitmap::Color COLOR_BG(15, 0, 0, 2);
    Bitmap::Color COLOR_GRID(15, 1, 2, 5);
    Bitmap::Color COLOR_GRID_2(15, 1, 4, 8);
    Bitmap::Color COLOR_WHITE = Bitmap::Color::White;
    Bitmap::Color COLOR_RED = Bitmap::Color::Red;
    Bitmap::Color COLOR_GREEN = Bitmap::Color::Green;
    Bitmap::Color COLOR_BLUE = Bitmap::Color::Blue;
    Bitmap::Color COLOR_YELLOW = Bitmap::Color::Yellow;
    Bitmap::Color COLOR_CYAN = Bitmap::Color::Cyan;
    Bitmap::Color COLOR_MAGENTA = Bitmap::Color::Magenta;
    Bitmap::Color COLOR_GRAY = Bitmap::Color::Gray;

    // -------------------------------------------------------------------------
    // GLOBAL STATE
    // -------------------------------------------------------------------------

    Sprite player;
    Sprite drone;
    Sprite satellite;

    Vector2f player_position(
        static_cast<float>(CENTER_X),
        static_cast<float>(CENTER_Y));

    Vector2f player_velocity(0.0f, 0.0f);

    float time_seconds = 0.0f;
    float pulse_time = 0.0f;

    bool paused = false;
    bool debug_mode = true;

    // Sprite data loaded by the engine.
    std::vector<Engine::Frame> sprite_data;

    void debug_frame_tick()
    {
        ++debug_frame;

        if (ARES_DEMO_DEBUG_LEVEL < 2)
            return;

        if ((debug_frame % DEBUG_FRAME_INTERVAL) != 0)
            return;

        const auto now = std::chrono::steady_clock::now();
        const double elapsed =
            std::chrono::duration<double>(now - debug_start_time).count();

        const double fps =
            elapsed > 0.0
                ? static_cast<double>(debug_frame) / elapsed
                : 0.0;

        DEBUG_LOG(
            "frame=%llu time=%.2f player=(%.2f, %.2f) velocity=(%.2f, %.2f) "
            "fps=%.1f paused=%s debug=%s",
            static_cast<unsigned long long>(debug_frame),
            static_cast<double>(time_seconds),
            static_cast<double>(player_position.x),
            static_cast<double>(player_position.y),
            static_cast<double>(player_velocity.x),
            static_cast<double>(player_velocity.y),
            fps,
            paused ? "yes" : "no",
            debug_mode ? "on" : "off");
    }

    // -------------------------------------------------------------------------
    // SMALL HELPERS
    // -------------------------------------------------------------------------

    int to_int(float value)
    {
        return static_cast<int>(value);
    }

    Vector2i to_vector2i(const Vector2f &v)
    {
        return Vector2i(to_int(v.x), to_int(v.y));
    }

    float sin01(float value)
    {
        return (std::sin(value) + 1.0f) * 0.5f;
    }

    // -------------------------------------------------------------------------
    // DRAWING
    // -------------------------------------------------------------------------

    void clear_screen()
    {
        Bitmap::set_rectangle(
            Vector2i(0, 0),
            Vector2i(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1),
            COLOR_BG);
    }

    void draw_grid()
    {
        constexpr int spacing = 16;

        for (int x = 0; x < SCREEN_WIDTH; x += spacing)
        {
            Bitmap::set_line(
                Vector2i(x, 0),
                Vector2i(x, SCREEN_HEIGHT - 1),
                COLOR_GRID);
        }

        for (int y = 0; y < SCREEN_HEIGHT; y += spacing)
        {
            Bitmap::set_line(
                Vector2i(0, y),
                Vector2i(SCREEN_WIDTH - 1, y),
                COLOR_GRID);
        }

        // Stronger center lines.
        Bitmap::set_line(
            Vector2i(CENTER_X, 0),
            Vector2i(CENTER_X, SCREEN_HEIGHT - 1),
            COLOR_GRID_2);

        Bitmap::set_line(
            Vector2i(0, CENTER_Y),
            Vector2i(SCREEN_WIDTH - 1, CENTER_Y),
            COLOR_GRID_2);
    }

    void draw_crosshair(
        const Vector2i &position,
        const Bitmap::Color &color,
        int size = 5)
    {
        Bitmap::set_line(
            Vector2i(position.x - size, position.y),
            Vector2i(position.x + size, position.y),
            color);

        Bitmap::set_line(
            Vector2i(position.x, position.y - size),
            Vector2i(position.x, position.y + size),
            color);
    }

    void draw_box(
        const Vector2i &center,
        int half_width,
        int half_height,
        const Bitmap::Color &color)
    {
        Bitmap::set_line(
            Vector2i(center.x - half_width, center.y - half_height),
            Vector2i(center.x + half_width, center.y - half_height),
            color);

        Bitmap::set_line(
            Vector2i(center.x + half_width, center.y - half_height),
            Vector2i(center.x + half_width, center.y + half_height),
            color);

        Bitmap::set_line(
            Vector2i(center.x + half_width, center.y + half_height),
            Vector2i(center.x - half_width, center.y + half_height),
            color);

        Bitmap::set_line(
            Vector2i(center.x - half_width, center.y + half_height),
            Vector2i(center.x - half_width, center.y - half_height),
            color);
    }

    void draw_radar()
    {
        const Vector2i center(CENTER_X, CENTER_Y);

        constexpr int radius_a = 30;
        constexpr int radius_b = 48;
        constexpr int radius_c = 66;

        // Approximate circles using line segments.
        constexpr int segments = 32;

        auto draw_circle = [](
                               const Vector2i &center,
                               int radius,
                               const Bitmap::Color &color)
        {
            for (int i = 0; i < segments; ++i)
            {
                float a0 = degrees_to_radians(
                    (360.0f / segments) * i);

                float a1 = degrees_to_radians(
                    (360.0f / segments) * (i + 1));

                Vector2i p0(
                    center.x + to_int(std::cos(a0) * radius),
                    center.y + to_int(std::sin(a0) * radius));

                Vector2i p1(
                    center.x + to_int(std::cos(a1) * radius),
                    center.y + to_int(std::sin(a1) * radius));

                Bitmap::set_line(p0, p1, color);
            }
        };

        draw_circle(center, radius_a, COLOR_GRID_2);
        draw_circle(center, radius_b, COLOR_GRID);
        draw_circle(center, radius_c, COLOR_GRID);

        draw_crosshair(center, COLOR_CYAN, 4);
    }

    void draw_orbit(
        const Vector2f &center,
        float radius,
        float rotation,
        const Bitmap::Color &color)
    {
        constexpr int segments = 48;

        for (int i = 0; i < segments; ++i)
        {
            float a0 =
                rotation +
                degrees_to_radians(
                    (360.0f / segments) * i);

            float a1 =
                rotation +
                degrees_to_radians(
                    (360.0f / segments) * (i + 1));

            Vector2i p0(
                to_int(center.x + std::cos(a0) * radius),
                to_int(center.y + std::sin(a0) * radius));

            Vector2i p1(
                to_int(center.x + std::cos(a1) * radius),
                to_int(center.y + std::sin(a1) * radius));

            Bitmap::set_line(p0, p1, color);
        }
    }

    void draw_vector(
        const Vector2f &origin,
        const Vector2f &vector,
        const Bitmap::Color &color,
        float scale)
    {
        Vector2f end = origin + vector * scale;

        Bitmap::set_line(
            to_vector2i(origin),
            to_vector2i(end),
            color);
    }

    // -------------------------------------------------------------------------
    // PLAYER
    // -------------------------------------------------------------------------

    void update_player()
    {
        DEBUG_VERBOSE("Player update begin");

        Vector2f direction(0.0f, 0.0f);

        if (Input::is_key_down(KEY_W))
            direction.y -= 1.0f;

        if (Input::is_key_down(KEY_S))
            direction.y += 1.0f;

        if (Input::is_key_down(KEY_A))
            direction.x -= 1.0f;

        if (Input::is_key_down(KEY_D))
            direction.x += 1.0f;

        // Vector normalization is one of the useful Vector2f features.
        const float direction_length_squared = direction.length_squared();

        if (direction_length_squared > 0.0f)
            direction = direction.normalized();

        player_velocity = direction * PLAYER_SPEED;

        DEBUG_VERBOSE(
            "Player direction=(%.2f,%.2f) len2=%.3f velocity=(%.2f,%.2f)",
            static_cast<double>(direction.x),
            static_cast<double>(direction.y),
            static_cast<double>(direction_length_squared),
            static_cast<double>(player_velocity.x),
            static_cast<double>(player_velocity.y));

        player_position += player_velocity;

        // Clamp to screen.
        player_position.x = clamp(
            player_position.x,
            10.0f,
            static_cast<float>(SCREEN_WIDTH - 10));

        player_position.y = clamp(
            player_position.y,
            10.0f,
            static_cast<float>(SCREEN_HEIGHT - 10));

        DEBUG_VERBOSE(
            "Player position=(%.2f,%.2f)",
            static_cast<double>(player_position.x),
            static_cast<double>(player_position.y));

        player.set_position(to_vector2i(player_position));

        // Demonstrate sprite flipping.
        if (player_velocity.x < 0.0f)
            player.set_flip(true, false);
        else if (player_velocity.x > 0.0f)
            player.set_flip(false, false);
    }

    // -------------------------------------------------------------------------
    // ORBITING OBJECTS
    // -------------------------------------------------------------------------

    void update_satellites()
    {
        DEBUG_VERBOSE("Satellite update begin");

        Vector2f center(
            static_cast<float>(CENTER_X),
            static_cast<float>(CENTER_Y));

        // Use radians/degrees helpers + sin/cos.
        float angle_a =
            degrees_to_radians(time_seconds * 75.0f);

        float angle_b =
            degrees_to_radians(-time_seconds * 45.0f);

        Vector2f drone_position(
            center.x + std::cos(angle_a) * 70.0f,
            center.y + std::sin(angle_a) * 70.0f);

        Vector2f satellite_position(
            center.x + std::cos(angle_b) * 105.0f,
            center.y + std::sin(angle_b) * 105.0f);

        drone.set_position(to_vector2i(drone_position));
        satellite.set_position(to_vector2i(satellite_position));

        DEBUG_VERBOSE(
            "Orbit positions: drone=(%.1f,%.1f) satellite=(%.1f,%.1f)",
            static_cast<double>(drone_position.x),
            static_cast<double>(drone_position.y),
            static_cast<double>(satellite_position.x),
            static_cast<double>(satellite_position.y));

        // Rotate through sprite frames if enough frames exist.
        if (!sprite_data.empty())
        {
            const uint8_t frame_count =
                static_cast<uint8_t>(sprite_data.size());

            DEBUG_VERBOSE(
                "Sprite animation frames=%u",
                static_cast<unsigned>(frame_count));

            player.set_frame(
                static_cast<uint8_t>(
                    static_cast<int>(time_seconds * 8.0f) % frame_count));

            drone.set_frame(
                static_cast<uint8_t>(
                    static_cast<int>(time_seconds * 6.0f) % frame_count));

            satellite.set_frame(
                static_cast<uint8_t>(
                    static_cast<int>(time_seconds * 4.0f) % frame_count));
        }
        else
        {
            DEBUG_INFO("Sprite animation skipped: no sprite frames loaded");
        }

        // Sprite scale demonstration.
        float pulse = sin01(pulse_time * 2.0f);

        if (pulse < 0.25f)
            drone.set_scale(Sprite::Scale::x1);
        else if (pulse < 0.50f)
            drone.set_scale(Sprite::Scale::x2);
        else if (pulse < 0.75f)
            drone.set_scale(Sprite::Scale::x4);
        else
            drone.set_scale(Sprite::Scale::x2);
    }

    // -------------------------------------------------------------------------
    // DEBUG / VECTOR VISUALIZATION
    // -------------------------------------------------------------------------

    void draw_debug()
    {
        if (!debug_mode)
            return;

        DEBUG_VERBOSE("Debug overlay draw");

        Vector2f center(
            static_cast<float>(CENTER_X),
            static_cast<float>(CENTER_Y));

        // Draw player velocity.
        draw_vector(
            player_position,
            player_velocity,
            COLOR_YELLOW,
            10.0f);

        // Draw direction from center -> player.
        Vector2f center_to_player =
            player_position - center;

        if (center_to_player.length_squared() > 0.0f)
        {
            Vector2f normalized_direction =
                center_to_player.normalized();

            draw_vector(
                center,
                normalized_direction,
                COLOR_MAGENTA,
                20.0f);
        }

        // Distance demonstration.
        float distance =
            Vector2f::distance(center, player_position);

        DEBUG_VERBOSE(
            "Debug distance=%.3f",
            static_cast<double>(distance));

        float mapped_distance = map(
            distance,
            0.0f,
            150.0f,
            0.0f,
            15.0f);

        mapped_distance =
            clamp(mapped_distance, 0.0f, 15.0f);

        // Visual "distance meter".
        for (int i = 0; i < 15; ++i)
        {
            Bitmap::Color color =
                (i < static_cast<int>(mapped_distance))
                    ? COLOR_GREEN
                    : COLOR_GRID_2;

            Bitmap::set_rectangle(
                Vector2i(8 + i * 7, SCREEN_HEIGHT - 12),
                Vector2i(12 + i * 7, SCREEN_HEIGHT - 7),
                color);
        }

        // Player bounds.
        draw_box(
            player.get_position(),
            10,
            10,
            COLOR_CYAN);
    }

    // -------------------------------------------------------------------------
    // ENGINE SETUP
    // -------------------------------------------------------------------------

    void initialize_engine()
    {
        debug_start_time = std::chrono::steady_clock::now();

        DEBUG_INFO("================ ARES CONCEPT DEMO ================");
        DEBUG_INFO("Debug level=%d", ARES_DEMO_DEBUG_LEVEL);
        DEBUG_INFO(
            "Screen=%dx%d center=(%d,%d) player_speed=%.2f",
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            CENTER_X,
            CENTER_Y,
            static_cast<double>(PLAYER_SPEED));

        // Enable the layers that this concept demo actually uses.
        DEBUG_INFO("Engine: enable Sprite");
        Engine::enable_layer(Engine::Layer::Sprite);

        DEBUG_INFO("Engine: enable Bitmap");
        Engine::enable_layer(Engine::Layer::Bitmap);

        // Tile layers are enabled as part of the feature demonstration.
        DEBUG_INFO("Engine: enable Tile0");
        Engine::enable_layer(Engine::Layer::Tile0);

        DEBUG_INFO("Engine: enable Tile1");
        Engine::enable_layer(Engine::Layer::Tile1);

        // ---------------------------------------------------------------------
        // Sprite assets
        // ---------------------------------------------------------------------
        //
        // Replace these paths with your actual files.
        //
        // The engine API exposes:
        //   load_sprite_data()
        //   load_sprite_palette()
        //
        // The demo assumes a valid sprite file exists.
        //

        const std::filesystem::path sprite_path =
            "assets/test.spr";

        const std::filesystem::path palette_path =
            "assets/test1.pal";

        DEBUG_INFO(
            "Assets: sprite=%s",
            sprite_path.string().c_str());

        if (std::filesystem::exists(sprite_path))
        {
            DEBUG_INFO("Assets: sprite file found");

            sprite_data =
                Engine::load_sprite_data(sprite_path);

            DEBUG_INFO(
                "Assets: loaded %llu sprite frames",
                static_cast<unsigned long long>(sprite_data.size()));
        }
        else
        {
            DEBUG_ERROR(
                "Assets: sprite file NOT FOUND: %s",
                sprite_path.string().c_str());
        }

        DEBUG_INFO(
            "Assets: palette=%s",
            palette_path.string().c_str());

        if (std::filesystem::exists(palette_path))
        {
            DEBUG_INFO("Assets: palette file found");

            Engine::load_sprite_palette(palette_path);

            DEBUG_INFO("Assets: palette load complete");
        }
        else
        {
            DEBUG_ERROR(
                "Assets: palette file NOT FOUND: %s",
                palette_path.string().c_str());
        }

        // ---------------------------------------------------------------------
        // Player
        // ---------------------------------------------------------------------

        player.set_position(
            to_vector2i(player_position));

        player.set_size(Sprite::Size::x32);
        player.set_scale(Sprite::Scale::x1);
        player.set_palette(Sprite::Palette::Pal_0);

        player.set_flip(false, false);
        player.set_visible(true);

        // ---------------------------------------------------------------------
        // Drone
        // ---------------------------------------------------------------------

        drone.set_position(
            Vector2i(CENTER_X + 70, CENTER_Y));

        drone.set_size(Sprite::Size::x16);
        drone.set_scale(Sprite::Scale::x1);
        drone.set_palette(Sprite::Palette::Pal_1);

        drone.set_flip(false, false);
        drone.set_visible(true);

        // ---------------------------------------------------------------------
        // Satellite
        // ---------------------------------------------------------------------

        satellite.set_position(
            Vector2i(CENTER_X + 105, CENTER_Y));

        satellite.set_size(Sprite::Size::x16);
        satellite.set_scale(Sprite::Scale::x1);
        satellite.set_palette(Sprite::Palette::Pal_2);

        satellite.set_visible(true);

        // ---------------------------------------------------------------------
        // Input
        // ---------------------------------------------------------------------

        DEBUG_INFO("Input: initialize()");

        if (!Input::initialize())
        {
            DEBUG_ERROR("Input initialization FAILED");
        }
        else
        {
            DEBUG_INFO("Input initialization OK");
        }
    }

    // -------------------------------------------------------------------------
    // INPUT
    // -------------------------------------------------------------------------

    void update_input()
    {
        Input::update(true);

        if (Input::is_key_pressed(KEY_ESC))
        {
            paused = !paused;
            ++debug_input_events;

            DEBUG_INFO(
                "Input: ESC -> paused=%s event=%llu",
                paused ? "yes" : "no",
                static_cast<unsigned long long>(debug_input_events));
        }

        if (Input::is_key_pressed(KEY_TAB))
        {
            debug_mode = !debug_mode;
            ++debug_input_events;

            DEBUG_INFO(
                "Input: TAB -> debug_mode=%s event=%llu",
                debug_mode ? "on" : "off",
                static_cast<unsigned long long>(debug_input_events));
        }

        if (Input::is_key_pressed(KEY_1))
        {
            player.set_palette(Sprite::Palette::Pal_0);
            ++debug_input_events;

            DEBUG_INFO(
                "Input: 1 -> player palette=0 event=%llu",
                static_cast<unsigned long long>(debug_input_events));
        }

        if (Input::is_key_pressed(KEY_2))
        {
            player.set_palette(Sprite::Palette::Pal_1);
            ++debug_input_events;

            DEBUG_INFO(
                "Input: 2 -> player palette=1 event=%llu",
                static_cast<unsigned long long>(debug_input_events));
        }

        if (Input::is_key_pressed(KEY_3))
        {
            player.set_palette(Sprite::Palette::Pal_2);
            ++debug_input_events;

            DEBUG_INFO(
                "Input: 3 -> player palette=2 event=%llu",
                static_cast<unsigned long long>(debug_input_events));
        }

        if (Input::is_key_pressed(KEY_4))
        {
            player.set_palette(Sprite::Palette::Pal_3);
            ++debug_input_events;

            DEBUG_INFO(
                "Input: 4 -> player palette=3 event=%llu",
                static_cast<unsigned long long>(debug_input_events));
        }

        // Sprite visibility demo.
        if (Input::is_key_pressed(KEY_SPACE))
        {
            drone.set_visible(!drone.get_visible());
            ++debug_input_events;

            DEBUG_INFO(
                "Input: SPACE -> drone visible=%s event=%llu",
                drone.get_visible() ? "yes" : "no",
                static_cast<unsigned long long>(debug_input_events));
        }
    }

    // -------------------------------------------------------------------------
    // FRAME
    // -------------------------------------------------------------------------

    void render()
    {
        clear_screen();
        draw_grid();
        draw_radar();

        Vector2f center(
            static_cast<float>(CENTER_X),
            static_cast<float>(CENTER_Y));

        // Dynamic orbital rings.
        draw_orbit(
            center,
            70.0f,
            degrees_to_radians(time_seconds * 15.0f),
            COLOR_BLUE);

        draw_orbit(
            center,
            105.0f,
            degrees_to_radians(-time_seconds * 10.0f),
            COLOR_MAGENTA);

        draw_debug();

        // Bitmap API and Sprite API intentionally coexist here.
        player.update();
        drone.update();
        satellite.update();
    }
}

// ============================================================================
// START
// ============================================================================

extern "C" void start(int argc, char **argv)
{
    DEBUG_INFO("start(argc=%d, argv=%p)", argc, static_cast<void *>(argv));

    initialize_engine();

    DEBUG_INFO("start() complete");
}

// ============================================================================
// LOOP
// ============================================================================

extern "C" void loop()
{
    debug_frame_tick();

    // Wait for the previous frame to finish.
    while (vdp_is_v_blank())
        ;

    while (!vdp_is_v_blank())
        ;

    // ------------------------------------------------------------------------
    // INPUT
    // ------------------------------------------------------------------------

    update_input();

    // ------------------------------------------------------------------------
    // LOGIC
    // ------------------------------------------------------------------------

    if (!paused)
    {
        update_player();
        update_satellites();

        time_seconds += 1.0f / 60.0f;
        pulse_time += 1.0f / 60.0f;
    }
    else
    {
        DEBUG_VERBOSE("Loop: simulation paused");
    }

    // ------------------------------------------------------------------------
    // RENDER
    // ------------------------------------------------------------------------

    render();
}