#ifndef __CORE_IMAGE_COLOR_H__
#define __CORE_IMAGE_COLOR_H__

struct RGBA32
{
    uint8_t r, g, b, a;
};

struct Colorf
{
    float r, g, b, a;

    Colorf();
    explicit Colorf(float s);
    explicit Colorf(float r, float g, float b, float a);

    Colorf operator+(const Colorf& v) const;
    Colorf operator-(const Colorf& v) const;
    Colorf operator*(float d) const;
    Colorf operator/(float d) const;

    Colorf operator+=(const Colorf& v);
    Colorf operator*=(float d);
    Colorf operator/=(float d);

    Colorf operator*(double d) const;
    Colorf operator/(double d) const;

    Colorf operator*=(double d);
    Colorf operator/=(double d);

    Colorf operator+(double d) const;
    Colorf operator-(double d) const;
};

struct Colord
{
    double r, g, b, a;

    Colord();
    explicit Colord(double s);
    explicit Colord(double r, double g, double b, double a);

    Colord operator+(const Colord& v) const;
    Colord operator-(const Colord& v) const;
    Colord operator*(double d) const;
    Colord operator/(double d) const;

    Colord operator+=(const Colord& v);
    Colord operator*=(double d);
    Colord operator/=(double d);

    Colord operator+(double d) const;
    Colord operator-(double d) const;
};
#include "Color.inl"

#endif // __CORE_IMAGE_COLOR_H__
