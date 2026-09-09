#include <ARES_Engine/math/vector2f.hpp>
#include <cmath>

namespace ARES_Engine
{

    float Vector2f::length() const
    {
        return std::sqrt(x * x + y * y);
    }

    float Vector2f::length_squared() const
    {
        return x * x + y * y;
    }

    Vector2f Vector2f::normalized() const
    {
        float len = length();

        if (len == 0.0f)
            return Vector2f(0.0f, 0.0f);

        return Vector2f(x / len, y / len);
    }

    float Vector2f::dot(const Vector2f &other) const
    {
        return x * other.x + y * other.y;
    }

    float Vector2f::distance(const Vector2f &a, const Vector2f &b)
    {
        return (a - b).length();
    }

    Vector2f Vector2f::operator+(const Vector2f &other) const
    {
        return Vector2f(x + other.x, y + other.y);
    }

    Vector2f Vector2f::operator-(const Vector2f &other) const
    {
        return Vector2f(x - other.x, y - other.y);
    }

    Vector2f Vector2f::operator*(float scalar) const
    {
        return Vector2f(x * scalar, y * scalar);
    }

    Vector2f Vector2f::operator/(float scalar) const
    {
        return Vector2f(x / scalar, y / scalar);
    }

    Vector2f &Vector2f::operator+=(const Vector2f &other)
    {
        x += other.x;
        y += other.y;

        return *this;
    }

    Vector2f &Vector2f::operator-=(const Vector2f &other)
    {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    Vector2f &Vector2f::operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;

        return *this;
    }

    Vector2f &Vector2f::operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;

        return *this;
    }
}
