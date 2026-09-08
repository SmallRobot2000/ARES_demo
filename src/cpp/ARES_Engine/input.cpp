#include <cpp_include/ARES_Engine/input.hpp>

namespace ARES_Engine
{
    std::array<bool, KEY_MAX + 1> Input::m_keys_down{};
    std::array<bool, KEY_MAX + 1> Input::m_keys_pressed{};
    std::array<bool, KEY_MAX + 1> Input::m_keys_released{};

    bool Input::initialize()
    {
        if (keyboard_init() != 0)
            return false;

        m_keys_down.fill(false);
        m_keys_pressed.fill(false);
        m_keys_released.fill(false);

        return true;
    }

    void Input::update()
    {
        // Pressed and released only last for one frame.
        m_keys_pressed.fill(false);
        m_keys_released.fill(false);

        keyboard_event_t event;

        while (keyboard_get_event(&event) == 1)
        {
            if (event.key > KEY_MAX)
                continue;

            switch (event.value)
            {
            case 1:
                // Key pressed.
                m_keys_down[event.key] = true;
                m_keys_pressed[event.key] = true;
                break;

            case 2:
                // Key repeat.
                m_keys_down[event.key] = true;
                break;

            case 0:
                // Key released.
                m_keys_down[event.key] = false;
                m_keys_released[event.key] = true;
                break;

            default:
                break;
            }
        }
    }

    bool Input::is_key_down(int key)
    {
        if (key < 0 || key > KEY_MAX)
            return false;

        return m_keys_down[key];
    }

    bool Input::is_key_pressed(int key)
    {
        if (key < 0 || key > KEY_MAX)
            return false;

        return m_keys_pressed[key];
    }

    bool Input::is_key_released(int key)
    {
        if (key < 0 || key > KEY_MAX)
            return false;

        return m_keys_released[key];
    }

}