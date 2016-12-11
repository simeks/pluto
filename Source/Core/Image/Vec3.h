#ifndef __CORE_VEC3_H__
#define __CORE_VEC3_H__

template<typename T>
class Vec3
{
public:
    Vec3();
    explicit Vec3(T s);
    explicit Vec3(T x, T y, T z);

    template<typename T2>
    Vec3(const Vec3<T2>& v) : x(T(v.x)), y(T(v.y)), z(T(v.z)) {}

    T x, y, z;

    double length() const;
    double length_squared() const;

    Vec3<T> operator+(const Vec3<T>& v) const;
    Vec3<T> operator-(const Vec3<T>& v) const;

    Vec3<T> operator+(double d) const;
    Vec3<T> operator-(double d) const;

    // Element wise multiplication
    Vec3<T> operator*(const Vec3<T>& v) const;
    Vec3<T> operator*(double d) const;
    Vec3<T> operator/(double d) const;

    Vec3<T> operator+=(const Vec3<T>& v);
    Vec3<T> operator*=(double d);
    Vec3<T> operator/=(double d);

    const T& operator[](int index) const;
    T& operator[](int index);

    bool operator==(const Vec3<T>& v) const;
    bool operator!=(const Vec3<T>& v) const;

    static const Vec3<T> Zero;
    static const Vec3<T> One;
    static const Vec3<T> UnitX;
    static const Vec3<T> UnitY;
    static const Vec3<T> UnitZ;
};

template<typename T> const Vec3<T> Vec3<T>::Zero(0, 0, 0);
template<typename T> const Vec3<T> Vec3<T>::One(1, 1, 1);
template<typename T> const Vec3<T> Vec3<T>::UnitX(1, 0, 0);
template<typename T> const Vec3<T> Vec3<T>::UnitY(0, 1, 0);
template<typename T> const Vec3<T> Vec3<T>::UnitZ(0, 0, 1);

typedef Vec3<uint8_t> Vec3u8;
typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;
typedef Vec3<int> Vec3i;

#include "Vec3.inl"

#endif // __CORE_VEC3_H__
