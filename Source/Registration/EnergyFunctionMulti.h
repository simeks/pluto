#ifndef __REGISTRATION_ENERGY_FUNCTION_MULTI_H__
#define __REGISTRATION_ENERGY_FUNCTION_MULTI_H__

#include <Core/Image/Color.h>
#include <Core/Image/Image.h>
#include <Core/Image/Vec3.h>

#include "RegistrationEngine.h"

template<typename TImage>
class EnergyFunctionMulti
{
public:
    EnergyFunctionMulti();

    double unary_term(const Vec3i& p, const Vec3d& def);

    double binary_term(const Vec3i& p, const Vec3d& def1, const Vec3d& def2, const Vec3i& step);

    void set_images(const Image* fixed, const Image* moving, int pair_count);

    void set_regularization_weight(int i, double alpha);
    void set_regularization_weight(double alpha);

private:
    void create_lookup();

    static const int s_max_num_image_pairs = 4;

    double _regularization_weights[s_max_num_image_pairs];

    Vec3d _fixed_spacing;
    Vec3d _moving_spacing;

    std::vector<TImage> _fixed_image;
    std::vector<TImage> _moving_image;

    ImageFloat64 _reg_lookup;

    int _pair_count;
};

#include "EnergyFunctionMulti.inl"

#endif // __REGISTRATION_ENERGY_FUNCTION_MULTI_H__
