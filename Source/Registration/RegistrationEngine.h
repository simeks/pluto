#ifndef __REGISTRATION_ENGINE_H__
#define __REGISTRATION_ENGINE_H__

#include <Core/Image/Image.h>
#include <Core/Object/Object.h>

class ImageObject;
class Optimizer;
class RegistrationEngine : public Object
{
    DECLARE_OBJECT(RegistrationEngine, Object);

public:
    struct Params
    {
        std::vector<Image> fixed;
        std::vector<Image> moving;

        ImageUInt8 constraint_mask;
        ImageVec3d constraint_values;

        ImageVec3d starting_guess;
    };

    DECLARE_OBJECT_CONSTRUCTOR(RegistrationEngine);
    ~RegistrationEngine();

    void object_init() OVERRIDE;
    void object_python_init(const Tuple&, const Dict&) OVERRIDE;

    void set_constraints(ImageObject* values, ImageObject* mask);
    void set_starting_guess(ImageObject* starting_guess);

    ImageObject* execute(const Tuple& fixed, const Tuple& moving);

private:
    Optimizer* create_optimizer(const char* name, 
        image::PixelType image_type, const Dict& settings) const;

    void build_pyramid();
    double unit_sigma(const Image& img);

    Optimizer* _optimizer;

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
