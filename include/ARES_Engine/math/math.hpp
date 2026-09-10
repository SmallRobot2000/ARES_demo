#pragma once

namespace ARES_Engine
{
    constexpr float PI = 3.14159265358979323846f;

    /// @brief Clamps a value between a minimum and maximum.
    /// @param value Value to clamp.
    /// @param min Minimum allowed value.
    /// @param max Maximum allowed value.
    /// @return The clamped value.
    float clamp(float value, float min, float max);

    /// @brief Returns the smaller of two values.
    /// @param a First value.
    /// @param b Second value.
    /// @return The smaller value.
    float min(float a, float b);

    /// @brief Returns the larger of two values.
    ///
    /// @param a First value.
    /// @param b Second value.
    /// @return The larger value.
    float max(float a, float b);

    /// @brief Linearly interpolates between two values.
    /// @param a Starting value.
    /// @param b Ending value.
    /// @param t Interpolation amount.
    /// @return The interpolated value.
    float lerp(float a, float b, float t);

    /// @brief Converts an angle from degrees to radians.
    /// @param degrees Angle in degrees.
    /// @return Angle in radians.
    float degrees_to_radians(float degrees);

    /// @brief Converts an angle from radians to degrees.
    ///
    /// @param radians Angle in radians.
    /// @return Angle in degrees.
    float radians_to_degrees(float radians);

    /// @brief Maps a value from one range to another.
    /// @param value Value to map.
    /// @param input_min Minimum of the input range.
    /// @param input_max Maximum of the input range.
    /// @param output_min Minimum of the output range.
    /// @param output_max Maximum of the output range.
    /// @return The value mapped to the output range.
    float map(
        float value,
        float input_min,
        float input_max,
        float output_min,
        float output_max);

}