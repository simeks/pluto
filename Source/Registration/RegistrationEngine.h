#ifndef __REGISTRATION_ENGINE_H__
#define __REGISTRATION_ENGINE_H__

#include <Core/Image/Image.h>
#include <Core/Python/Object.h>

class Optimizer;
class Tuple;
class RegistrationEngine
{
public:
    struct Params
    {
        std::vector<Image> fixed;
        std::vector<Image> moving;

        ImageUInt8 constraint_mask;
        ImageVec3d constraint_values;

        ImageVec3d starting_guess;
    };

    RegistrationEngine();
    RegistrationEngine(const Tuple& args);
    ~RegistrationEngine();

    void set_constraints(const Image& values, const Image& mask);
    void set_starting_guess(const Image& starting_guess);

    Image execute(const Tuple& fixed, const Tuple& moving);

    static python::Object python_class();

private:
    Optimizer* create_optimizer(const char* name, 
        image::PixelType image_type, const python::Dict& settings) const;

    void build_pyramid();
    double unit_sigma(const Image& img);

    Optimizer* _optimizer;
    image::PixelType _image_type;

    int _pyramid_level_min;
    int _pyramid_level_max;

    std::vector<std::vector<Image>> _fixed_pyramid;
    std::vector<std::vector<Image>> _moving_pyramid;
    std::vector<ImageVec3d> _deformation_pyramid;
    std::vector<ImageVec3d> _residual_pyramid;

    std::vector<ImageUInt8> _constraint_mask_pyramid;
    std::vector<ImageVec3d> _constraint_pyramid;

    int _image_pair_count;

    bool _normalize_images;
};


#endif // __REGISTRATION_ENGINE_H__
