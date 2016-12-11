
INLINE Colorf::Colorf() : r(0), g(0), b(0), a(0)
{
}
INLINE Colorf::Colorf(float s) : r(s), g(s), b(s), a(s)
{
}
INLINE Colorf::Colorf(float r, float g, float b, float a) : r(r), g(g), b(b), a(a)
{
}

INLINE Colorf Colorf::operator+(const Colorf& v) const
{
    return Colorf(r + v.r, g + v.g, b + v.b, a + v.a);
}
INLINE Colorf Colorf::operator-(const Colorf& v) const
{
    return Colorf(r - v.r, g - v.g, b - v.b, a - v.a);
}
INLINE Colorf Colorf::operator*(float d) const
{
    return Colorf(r * d, g * d, b * d, a * d);
}
INLINE Colorf Colorf::operator/(float d) const
{
    return Colorf(r / d, g / d, b / d, a / d);
}

INLINE Colorf Colorf::operator+=(const Colorf& v)
{
    r += v.r;
    g += v.g;
    b += v.b;
    a += v.a;
    return *this;
}
INLINE Colorf Colorf::operator*=(float d)
{
    r *= d;
    g *= d;
    b *= d;
    a *= d;
    return *this;
}
INLINE Colorf Colorf::operator/=(float d)
{
    r /= d;
    g /= d;
    b /= d;
    a /= d;
    return *this;
}

INLINE Colorf operator*(float d, const Colorf v)
{
    return v.operator*(d);
}

INLINE Colorf Colorf::operator*(double d) const
{
    return operator*(float(d));
}
INLINE Colorf Colorf::operator/(double d) const
{
    return operator/(float(d));
}

INLINE Colorf Colorf::operator*=(double d)
{
    return operator*=(float(d));
}
INLINE Colorf Colorf::operator/=(double d)
{
    return operator/=(float(d));
}
INLINE Colorf Colorf::operator+(double d) const
{
    return Colorf(float(r + d), float(g + d), float(b + d), float(a + d));
}
INLINE Colorf Colorf::operator-(double d) const
{
    return Colorf(float(r - d), float(g - d), float(b - d), float(a - d));
}

INLINE Colorf operator*(double d, const Colorf v)
{
    return v.operator*(d);
}

INLINE Colord::Colord() : r(0), g(0), b(0), a(0)
{
}
INLINE Colord::Colord(double s) : r(s), g(s), b(s), a(s)
{
}
INLINE Colord::Colord(double r, double g, double b, double a) : r(r), g(g), b(b), a(a)
{
}

INLINE Colord Colord::operator+(const Colord& v) const
{
    return Colord(r + v.r, g + v.g, b + v.b, a + v.a);
}
INLINE Colord Colord::operator-(const Colord& v) const
{
    return Colord(r - v.r, g - v.g, b - v.b, a - v.a);
}
INLINE Colord Colord::operator*(double d) const
{
    return Colord(r * d, g * d, b * d, a * d);
}
INLINE Colord Colord::operator/(double d) const
{
    return Colord(r / d, g / d, b / d, a / d);
}

INLINE Colord Colord::operator+=(const Colord& v)
{
    r += v.r;
    g += v.g;
    b += v.b;
    a += v.a;
    return *this;
}
INLINE Colord Colord::operator*=(double d)
{
    r *= d;
    g *= d;
    b *= d;
    a *= d;
    return *this;
}
INLINE Colord Colord::operator/=(double d)
{
    r /= d;
    g /= d;
    b /= d;
    a /= d;
    return *this;
}
INLINE Colord Colord::operator+(double d) const
{
    return Colord(r + d, g + d, b + d, a + d);
}
INLINE Colord Colord::operator-(double d) const
{
    return Colord(r - d, g - d, b - d, a - d);
}

INLINE Colord operator*(double d, const Colord v)
{
    return v.operator*(d);
}
