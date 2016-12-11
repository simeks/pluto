template<typename T>
Vec3<T>::Vec3() : x(0), y(0), z(0)
{
}
template<typename T>
Vec3<T>::Vec3(T s) : x(s), y(s), z(s)
{
}
template<typename T>
Vec3<T>::Vec3(T x, T y, T z) : x(x), y(y), z(z)
{
}

template<typename T>
double Vec3<T>::length() const
{
    return sqrt(x*x + y*y + z*z);
}
template<typename T>
double Vec3<T>::length_squared() const
{
    return x*x + y*y + z*z;
}
template<typename T>
Vec3<T> Vec3<T>::operator+(const Vec3<T>& v) const
{
    return Vec3<T>(x + v.x, y + v.y, z + v.z);
}
template<typename T>
Vec3<T> Vec3<T>::operator-(const Vec3<T>& v) const
{
    return Vec3<T>(x - v.x, y - v.y, z - v.z);
}
template<typename T>
Vec3<T> Vec3<T>::operator+(double d) const
{
    return Vec3<T>(T(x + d), T(y + d), T(z + d));
}
template<typename T>
Vec3<T> Vec3<T>::operator-(double d) const
{
    return Vec3<T>(T(x - d), T(y - d), T(z - d));
}
template<typename T>
Vec3<T> Vec3<T>::operator*(const Vec3<T>& v) const
{
    return Vec3<T>(x * v.x, y * v.y, z * v.z);
}
template<typename T>
Vec3<T> Vec3<T>::operator*(double d) const
{
    return Vec3<T>(T(x*d), T(y*d), T(z*d));
}
template<typename T>
Vec3<T> Vec3<T>::operator/(double d) const
{
    return Vec3<T>(T(x / d), T(y / d), T(z / d));
}
template<typename T>
Vec3<T> Vec3<T>::operator+=(const Vec3<T>& v)
{
    x += v.x; y += v.y; z += v.z;
    return *this;
}
template<typename T>
Vec3<T> Vec3<T>::operator*=(double d)
{
    x *= d; y *= d; z *= d;
    return *this;
}
template<typename T>
Vec3<T> Vec3<T>::operator/=(double d)
{
    x /= d; y /= d; z /= d;
    return *this;
}
template<typename T>
const T& Vec3<T>::operator[](int index) const
{
    assert(index < 3);
    return *(&x + index);
}
template<typename T>
T& Vec3<T>::operator[](int index)
{
    assert(index < 3);
    return *(&x + index);
}

template<typename T>
bool Vec3<T>::operator==(const Vec3<T>& v) const
{
    return (x == v.x && y == v.y && z == v.z);
}

template<typename T>
bool Vec3<T>::operator!=(const Vec3<T>& v) const
{
    return !((*this) == v);
}

template<typename T>
Vec3<T> operator*(double d, const Vec3<T>& v)
{
    return v.operator*(d);
}
