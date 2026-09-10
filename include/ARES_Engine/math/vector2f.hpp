#pragma once

namespace ARES_Engine
{
    struct Vector2f
    {
        float x = 0.0f;
        float y = 0.0f;

        Vector2f() = default;
        Vector2f(float x, float y) : x(x), y(y) {}

        /// @brief Calculates the length (magnitude) of the vector.
        /// @return The length of the vector.
        float length() const;

        /// @brief Calculates the squared length (magnitude) of the vector.
        /// This avoids the square root calculation performed by length().
        /// @return The squared length of the vector.
        float length_squared() const;

        /// @brief Returns a normalized copy of this vector.
        /// A normalized vector has a length of 1 while preserving the original direction.
        /// @return The normalized vector.
        Vector2f normalized() const;

        /// @brief Calculates the dot product between two vectors.
        /// @param other Vector to calculate the dot product with.
        /// @return The dot product of the two vectors.
        float dot(const Vector2f &other) const;

        /// @brief Calculates the distance between two vectors.
        /// @param a First vector.
        /// @param b Second vector.
        /// @return The distance between the two vectors.
        static float distance(const Vector2f &a, const Vector2f &b);

        Vector2f operator+(const Vector2f &other) const;
        Vector2f operator-(const Vector2f &other) const;

        Vector2f operator*(float scalar) const;
        Vector2f operator/(float scalar) const;

        Vector2f &operator+=(const Vector2f &other);
        Vector2f &operator-=(const Vector2f &other);

        Vector2f &operator*=(float scalar);
        Vector2f &operator/=(float scalar);
    };
}