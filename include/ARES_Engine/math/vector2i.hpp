#pragma once
#include <ARES_Engine/math/vector2f.hpp>

namespace ARES_Engine
{
    struct Vector2i
    {
        int x = 0;
        int y = 0;

        Vector2i() = default;
        Vector2i(int x, int y) : x(x), y(y) {}

        /// @brief Calculates the length (magnitude) of the vector.
        /// @return The length of the vector.
        float length() const;

        /// @brief Calculates the squared length (magnitude) of the vector.
        /// This avoids the square root calculation performed by length().
        /// @return The squared length of the vector.
        float length_squared() const;

        /// @brief Returns a normalized copy of this vector.
        /// A normalized vector has a length of 1 while preserving the original direction.
        /// @return The normalized vector as a ARES_Engine::Vector2f.
        Vector2f normalized() const;

        /// @brief Calculates the dot product between two vectors.
        /// @param other Vector to calculate the dot product with.
        /// @return The dot product of the two vectors.
        int dot(const Vector2i &other) const;

        /// @brief Calculates the distance between two vectors.
        /// @param a First vector.
        /// @param b Second vector.
        /// @return The distance between the two vectors.
        static float distance(const Vector2i &a, const Vector2i &b);

        Vector2i operator+(const Vector2i &other) const;
        Vector2i operator-(const Vector2i &other) const;

        Vector2i operator*(int scalar) const;
        Vector2i operator/(int scalar) const;

        Vector2i &operator+=(const Vector2i &other);
        Vector2i &operator-=(const Vector2i &other);

        Vector2i &operator*=(int scalar);
        Vector2i &operator/=(int scalar);
    };
}