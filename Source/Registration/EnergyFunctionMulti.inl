
template<typename TImage>
EnergyFunctionMulti<TImage>::EnergyFunctionMulti() :
    _pair_count(0)
{
    for (int i = 0; i < s_max_num_image_pairs; ++i)
        _regularization_weights[i] = 0.05;
}

template<typename TImage>
INLINE double EnergyFunctionMulti<TImage>::unary_term(const Vec3i& p, const Vec3d& def)
{
    double dataterm = 0;
    for (long i = 0; i < _pair_count; i++)
    {
        dataterm += pow(fabs(_fixed_image[i](p) -
            _moving_image[i].linear_at(Vec3d(p) + def)), 2);
    }

    return (1 - _reg_lookup(p))*dataterm;
}
template<typename TImage>
INLINE double EnergyFunctionMulti<TImage>::binary_term(const Vec3i& p, const Vec3d& def1, const Vec3d& def2, const Vec3i& step)
{
    Vec3d diff = (def1 - def2) * _moving_spacing;

    double n = (step * _fixed_spacing).length_squared();
    return _reg_lookup(p)*diff.length_squared() / n; // TODO: Spacing?
}
template<typename TImage>
void EnergyFunctionMulti<TImage>::set_images(const Image* fixed_image, const Image* moving_image, int pair_count)
{
    assert(fixed_image && moving_image);
    assert(pair_count > 0);
    assert(pair_count <= s_max_num_image_pairs);

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

    create_lookup();
}
template <typename TImage>
void EnergyFunctionMulti<TImage>::create_lookup()
{
    _reg_lookup = ImageFloat64(_fixed_image[0].ndims(), _fixed_image[0].size(), 0.0);
    for (int z = 0; z < _fixed_image[0].size().z; ++z)
    {
        for (int y = 0; y < _fixed_image[0].size().y; ++y)
        {
            for (int x = 0; x < _fixed_image[0].size().x; ++x)
            {
                double weight = 0.0;
                double sum = 0.0;
                for (int i = 0; i < _pair_count; ++i)
                {
                    sum += _fixed_image[i](x, y, z);
                }
                for (int i = 0; i < _pair_count; ++i)
                {
                    weight += _regularization_weights[i] * (_fixed_image[i](x, y, z) / sum);
                }
                std::cout << "Weight: " << weight << std::endl;
                _reg_lookup(x, y, z) = weight;
            }
        }
    }
}
template <>
void EnergyFunctionMulti<ImageColorf>::create_lookup()
{
    assert(false); // Not implemented
}
template <typename TImage>
INLINE void EnergyFunctionMulti<TImage>::set_regularization_weight(int i, double alpha)
{
    _regularization_weights[i] = alpha;
}
template <typename TImage>
INLINE void EnergyFunctionMulti<TImage>::set_regularization_weight(double alpha)
{
    for (int i = 0; i < s_max_num_image_pairs; ++i)
        _regularization_weights[i] = alpha;
}
template<>
INLINE double EnergyFunctionMulti<ImageColorf>::unary_term(const Vec3i& , const Vec3d& )
{
    assert(false); // Not implemented
    return 0.0;
}
