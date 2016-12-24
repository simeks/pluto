template<typename T>
Vec2<T>::Vec2() : x(0), y(0)
{
}
template<typename T>
Vec2<T>::Vec2(T s) : x(s), y(s)
{
}
template<typename T>
Vec2<T>::Vec2(T x, T y) : x(x), y(y)
{
}

template<typename T>
double Vec2<T>::length() const
{
    return sqrt(x*x + y*y);
}
template<typename T>
double Vec2<T>::length_squared() const
{
    return x*x + y*y;
}
template<typename T>
Vec2<T> Vec2<T>::operator+(const Vec2<T>& v) const
{
    return Vec2<T>(x + v.x, y + v.y);
}
template<typename T>
Vec2<T> Vec2<T>::operator-(const Vec2<T>& v) const
{
    return Vec2<T>(x - v.x, y - v.y);
}
template<typename T>
Vec2<T> Vec2<T>::operator+(double d) const
{
    return Vec2<T>(T(x + d), T(y + d));
}
template<typename T>
Vec2<T> Vec2<T>::operator-(double d) const
{
    return Vec2<T>(T(x - d), T(y - d));
}
template<typename T>
Vec2<T> Vec2<T>::operator*(const Vec2<T>& v) const
{
    return Vec2<T>(x * v.x, y * v.y);
}
template<typename T>
Vec2<T> Vec2<T>::operator*(double d) const
{
    return Vec2<T>(T(x*d), T(y*d));
}
template<typename T>
Vec2<T> Vec2<T>::operator/(double d) const
{
    return Vec2<T>(T(x / d), T(y / d));
}
template<typename T>
Vec2<T> Vec2<T>::operator+=(const Vec2<T>& v)
{
    x += v.x; y += v.y;
    return *this;
}
template<typename T>
Vec2<T> Vec2<T>::operator*=(double d)
{
    x *= d; y *= d;
    return *this;
}
template<typename T>
Vec2<T> Vec2<T>::operator/=(double d)
{
    x /= d; y /= d;
    return *this;
}
template<typename T>
const T& Vec2<T>::operator[](int index) const
{
    assert(index < 2);
    return *(&x + index);
}
template<typename T>
T& Vec2<T>::operator[](int index)
{
    assert(index < 2);
    return *(&x + index);
}

template<typename T>
bool Vec2<T>::operator==(const Vec2<T>& v) const
{
    return (x == v.x && y == v.y);
}

template<typename T>
bool Vec2<T>::operator!=(const Vec2<T>& v) const
{
    return !((*this) == v);
}

template<typename T>
Vec2<T> operator*(double d, const Vec2<T>& v)
{
    return v.operator*(d);
}
