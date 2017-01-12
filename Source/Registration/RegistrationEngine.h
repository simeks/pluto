#ifndef __REGISTRATION_ENGINE_H__
#define __REGISTRATION_ENGINE_H__

#include <Core/Image/Image.h>

class Optimizer;
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

    RegistrationEngine(Optimizer* optimizer);
    ~RegistrationEngine();

    ImageVec3d execute(const Params& params);

private:
    void build_pyramid();
    double unit_sigma(const Image& img);

    Optimizer* _optimizer;

    int _pyramid_levels;
    int _pyramid_max_level;

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
