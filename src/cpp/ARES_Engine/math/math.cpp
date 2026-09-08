#include <cmath>
#include <cpp_include/ARES_Engine/math/math.hpp>

namespace ARES_Engine
{
    float clamp(float value, float min, float max)
    {
        if (value < min)
            return min;

        if (value > max)
            return max;

        return value;
    }

    float min(float a, float b)
    {
        return (a < b) ? a : b;
    }

    float max(float a, float b)
    {
        return (a > b) ? a : b;
    }

    float lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    float degrees_to_radians(float degrees)
    {
        return degrees * (PI / 180.0f);
    }

    float radians_to_degrees(float radians)
    {
        return radians * (180.0f / PI);
    }

    float map(
        float value,
        float input_min,
        float input_max,
        float output_min,
        float output_max)
    {
        return output_min +
               (value - input_min) *
                   (output_max - output_min) /
                   (input_max - input_min);
    }
}