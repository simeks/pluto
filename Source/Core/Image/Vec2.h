#ifndef __CORE_VEC2_H__
#define __CORE_VEC2_H__

template<typename T>
class Vec2
{
public:
    Vec2();
    explicit Vec2(T s);
    explicit Vec2(T x, T y);

    template<typename T2>
    Vec2(const Vec2<T2>& v) : x(T(v.x)), y(T(v.y)) {}

    T x, y;

    double length() const;
    double length_squared() const;

    Vec2<T> operator+(const Vec2<T>& v) const;
    Vec2<T> operator-(const Vec2<T>& v) const;

    Vec2<T> operator+(double d) const;
    Vec2<T> operator-(double d) const;

    // Element wise multiplication
    Vec2<T> operator*(const Vec2<T>& v) const;
    Vec2<T> operator*(double d) const;
    Vec2<T> operator/(double d) const;

    Vec2<T> operator+=(const Vec2<T>& v);
    Vec2<T> operator*=(double d);
    Vec2<T> operator/=(double d);

    const T& operator[](int index) const;
    T& operator[](int index);

    bool operator==(const Vec2<T>& v) const;
    bool operator!=(const Vec2<T>& v) const;

    static const Vec2<T> Zero;
    static const Vec2<T> One;
    static const Vec2<T> UnitX;
    static const Vec2<T> UnitY;
};

template<typename T> const Vec2<T> Vec2<T>::Zero(0, 0);
template<typename T> const Vec2<T> Vec2<T>::One(1, 1);
template<typename T> const Vec2<T> Vec2<T>::UnitX(1, 0);
template<typename T> const Vec2<T> Vec2<T>::UnitY(0, 1);

typedef Vec2<uint8_t> Vec2u8;
typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec2<int> Vec2i;

#include "Vec2.inl"

#endif // __CORE_VEC2_H__
