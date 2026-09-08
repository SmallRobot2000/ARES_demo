#include <linux/input.h>
#include <array>
#include <kbd.h>

namespace ARES_Engine
{
    class Input
    {
    public:
        /// @brief Initializes the keyboard input system.
        /// @return True if initialization succeeded, false otherwise.
        static bool initialize();

        /// @brief Updates the state of all keyboard keys.
        /// @brief This should be called once per frame.
        static void update();

        /// @brief Checks whether a key is currently held down.
        /// @param key Linux KEY_* code.
        /// @return True if the key is currently held.
        static bool is_key_down(int key);

        /// @brief Checks whether a key was pressed during the current frame.
        /// @param key Linux KEY_* code.
        /// @return True if the key was pressed this frame.
        static bool is_key_pressed(int key);

        /// @brief Checks whether a key was released during the current frame.
        /// @param key Linux KEY_* code.
        /// @return True if the key was released this frame.
        static bool is_key_released(int key);

        /// @brief Checks whether a key was released during the current frame.
        /// @param key Linux KEY_* code.
        /// @return True if the key was released this frame.
        static bool is_key_released(int key);

    private:
        // Internal state for key tracking
        // KEY_MAX is defined in linux/input.h and represents the maximum key code value.
        static std::array<bool, KEY_MAX + 1> m_keys_down;
        static std::array<bool, KEY_MAX + 1> m_keys_pressed;
        static std::array<bool, KEY_MAX + 1> m_keys_released;
    };
}
