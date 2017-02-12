
template<typename TImage>
EnergyFunction<TImage>::EnergyFunction() :
    _regulariation_weight(0.05),
    _pair_count(0)
{
}

template<typename TImage>
INLINE double EnergyFunction<TImage>::unary_term(const Vec3i& p, const Vec3d& def)
{
    double dataterm = 0;
    for (long i = 0; i < _pair_count; i++)
    {
        dataterm += pow(fabs(_fixed_image[i](p) -
            _moving_image[i].linear_at(Vec3d(p) + def)), 2);
    }

    return (1 - _regulariation_weight)*dataterm;
}
template<typename TImage>
INLINE double EnergyFunction<TImage>::binary_term(const Vec3i& /*p*/, const Vec3d& def1, const Vec3d& def2, const Vec3i& step)
{
    Vec3d diff = (def1 - def2) * _moving_spacing;

    double n = (step * _fixed_spacing).length_squared();
    return _regulariation_weight*diff.length_squared() / n;
}
template<typename TImage>
void EnergyFunction<TImage>::set_images(const Image* fixed_image, const Image* moving_image, int pair_count)
{
    assert(fixed_image && moving_image);
    assert(pair_count > 0);

    _pair_count = pair_count;
    _fixed_image.resize(_pair_count);
    _moving_image.resize(_pair_count);

    for (int i = 0; i < _pair_count; ++i)
    {
        _fixed_image[i] = fixed_image[i];
        _moving_image[i] = moving_image[i];
    }

    _fixed_spacing = _fixed_image[0].spacing();
    _moving_spacing = _moving_image[0].spacing();
}
template <typename TImage>
INLINE void EnergyFunction<TImage>::set_regularization_weight(double alpha)
{
    _regulariation_weight = alpha;
}
template<>
INLINE double EnergyFunction<ImageColorf>::unary_term(const Vec3i& p, const Vec3d& def)
{
    double dataterm = 0;
    for (long i = 0; i < _pair_count; i++)
    {
        Colorf diff = _fixed_image[i](p) - _moving_image[i].linear_at(Vec3d(p) + def);

        dataterm += diff.r*diff.r + diff.g*diff.g + diff.b*diff.b + diff.a*diff.a;
    }

    return (1 - _regulariation_weight)*dataterm;
}
