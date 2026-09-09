// For some reason i need .vscode c_cpp_properties.json -HELP
#include <ARES_Engine/math/vector2i.hpp>
#include <cmath>

namespace ARES_Engine
{

    float Vector2i::length() const
    {
        return std::sqrt(x * x + y * y);
    }

    float Vector2i::length_squared() const
    {
        return x * x + y * y;
    }

    Vector2f Vector2i::normalized() const
    {
        float len = length();

        if (len == 0.0f)
            return Vector2f(0.0f, 0.0f);

        return Vector2f(x / len, y / len);
    }

    int Vector2i::dot(const Vector2i &other) const
    {
        return x * other.x + y * other.y;
    }

    float Vector2i::distance(const Vector2i &a, const Vector2i &b)
    {
        return (a - b).length();
    }

    Vector2i Vector2i::operator+(const Vector2i &other) const
    {
        return Vector2i(x + other.x, y + other.y);
    }

    Vector2i Vector2i::operator-(const Vector2i &other) const
    {
        return Vector2i(x - other.x, y - other.y);
    }

    Vector2i Vector2i::operator*(int scalar) const
    {
        return Vector2i(x * scalar, y * scalar);
    }

    Vector2i Vector2i::operator/(int scalar) const
    {
        return Vector2i(x / scalar, y / scalar);
    }

    Vector2i &Vector2i::operator+=(const Vector2i &other)
    {
        x += other.x;
        y += other.y;

        return *this;
    }

    Vector2i &Vector2i::operator-=(const Vector2i &other)
    {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    Vector2i &Vector2i::operator*=(int scalar)
    {
        x *= scalar;
        y *= scalar;

        return *this;
    }

    Vector2i &Vector2i::operator/=(int scalar)
    {
        x /= scalar;
        y /= scalar;

        return *this;
    }
}
