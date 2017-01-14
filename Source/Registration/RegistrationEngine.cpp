#include <Core/Common.h>
#include <Core/Image/Image.h>
#include <Core/Image/ImageObject.h>
#include <Core/Python/PythonCommon.h>

#include "BlockedGraphCutOptimizer.h"
#include "Optimizer.h"
#include "RegistrationEngine.h"
#include "Resample.h"

namespace
{
    void find_min_max(const Image& img, double& min, double& max)
    {
        min = std::numeric_limits<double>::max();
        max = -std::numeric_limits<double>::max();
        if (img.pixel_type() == image::PixelType_Float32)
        {
            ImageFloat32 m(img);

            // TODO: step
            for (size_t i = 0; i < m.pixel_count(); ++i)
            {
                min = std::min<double>(m[i], min);
                max = std::max<double>(m[i], max);
            }
        }
        else if (img.pixel_type() == image::PixelType_Float64)
        {
            ImageFloat64 m(img);

            // TODO: step
            for (size_t i = 0; i < m.pixel_count(); ++i)
            {
                min = std::min<double>(m[i], min);
                max = std::max<double>(m[i], max);
            }
        }
        else
        {
            PYTHON_ERROR(TypeError, "Only float and double supported! (Type: %s)", image::pixel_type_to_string(img.pixel_type()));
        }
    }
}

template<typename TImage>
static TImage normalize_image(const TImage& image)
{
    double min, max;
    find_min_max(image, min, max);

    TImage result = image.clone();
    Vec3i dims = result.size();

    double d = max - min;
#pragma omp parallel for
    for (int y = 0; y < dims.y; y++)
    {
        for (int z = 0; z < dims.z; z++)
        {
            for (int x = 0; x < dims.x; x++)
            {
                result(x, y, z) = typename TImage::TPixelType((result(x,y,z) - min) / d);
            }
        }
    }
    return result;
}

static ImageUInt8 downsample_mask(const ImageUInt8& mask, double scale)
{
    assert(scale > 0.0 && scale < 1.0);
    double inv_scale = 1.0 / scale;

    std::vector<Vec3i> neighbors;

    neighbors.push_back(Vec3i(0, 0, 0));
    neighbors.push_back(Vec3i(1, 0, 0));
    neighbors.push_back(Vec3i(0, 1, 0));
    neighbors.push_back(Vec3i(0, 0, 1));
    neighbors.push_back(Vec3i(1, 0, 1));
    neighbors.push_back(Vec3i(1, 1, 0));
    neighbors.push_back(Vec3i(0, 1, 1));
    neighbors.push_back(Vec3i(1, 1, 1));

    Vec3i old_dims = mask.size();
    Vec3i new_dims(1, 1, 1);
    for (int i = 0; i < mask.ndims(); ++i)
    {
        new_dims[i] = (int)ceil(old_dims[i] * scale);
    }

    ImageUInt8 result(mask.ndims(), new_dims, (uint8_t)0);
    result.set_spacing(mask.spacing() * inv_scale);
    result.set_origin(mask.origin());

#pragma omp parallel for
    for (int z = 0; z < new_dims.z; ++z)
    {
        for (int y = 0; y < new_dims.y; ++y)
        {
            for (int x = 0; x < new_dims.x; ++x)
            {
                Vec3i v0 = inv_scale * Vec3i(x, y, z);
                uint8_t maxval = 0;
                for (int n = 0; n < int(neighbors.size()); n++)
                {
                    uint8_t tmpval = mask.at(v0 + neighbors[n], image::Border_Constant);
                    if (tmpval > maxval)
                        maxval = tmpval;
                }
                result(x, y, z) = maxval;
            }
        }
    }

    return result;
}

static ImageVec3d downsample_constraint_values(const ImageVec3d& values, const ImageUInt8& mask, double scale)
{
    assert(scale > 0.0 && scale < 1.0);
    double inv_scale = 1.0 / scale;

    std::vector<Vec3i> neighbors;

    neighbors.push_back(Vec3i(0, 0, 0));
    neighbors.push_back(Vec3i(1, 0, 0));
    neighbors.push_back(Vec3i(0, 1, 0));
    neighbors.push_back(Vec3i(0, 0, 1));
    neighbors.push_back(Vec3i(1, 0, 1));
    neighbors.push_back(Vec3i(1, 1, 0));
    neighbors.push_back(Vec3i(0, 1, 1));
    neighbors.push_back(Vec3i(1, 1, 1));

    Vec3i old_dims = values.size();
    Vec3i new_dims(1, 1, 1);
    for (int i = 0; i < values.ndims(); ++i)
    {
        new_dims[i] = (int)ceil(old_dims[i] * scale);
    }

    ImageVec3d result(values.ndims(), new_dims, Vec3d(0, 0, 0));
    result.set_spacing(values.spacing() * inv_scale);
    result.set_origin(values.origin());

#pragma omp parallel for
    for (int z = 0; z < new_dims.z; ++z)
    {
        for (int y = 0; y < new_dims.y; ++y)
        {
            for (int x = 0; x < new_dims.x; ++x)
            {
                Vec3i p = Vec3i(x, y, z);
                int nmask = 0;
                Vec3d val = Vec3d(0, 0, 0);
                for (auto& n : neighbors)
                {
                    if (mask.at(2 * p + n, image::Border_Constant) > 0)
                    {
                        nmask++;
                        val += values.at(2 * p + n, image::Border_Replicate);
                    }
                }

                if (nmask > 0)
                {
                    result(x, y, z) = 0.5*val / double(nmask);
                }
            }
        }
    }
    return result;
}

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2(RegistrationEngine, set_constraints, ImageObject*, ImageObject*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(RegistrationEngine, set_starting_guess, ImageObject*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2(RegistrationEngine, execute, Tuple, Tuple);

OBJECT_INIT_TYPE_FN(RegistrationEngine)
{
    OBJECT_PYTHON_ADD_METHOD(RegistrationEngine, set_constraints, 
        "set_constraints(values, mask=None)\n"
        "--\n"
        "Args:\n"
        "    value (Image) : Image of Vec3d containing displacement vectors\n"
        "    mask (Image) : Binary image specifying which voxels to constrain\n"
        "");
    OBJECT_PYTHON_ADD_METHOD(RegistrationEngine, set_starting_guess,
        "set_starting_guess(values)\n"
        "--\n"
        "Args:\n"
        "    value (Image) : Image of Vec3d with the initial deformation field\n"
        "");
    OBJECT_PYTHON_ADD_METHOD(RegistrationEngine, execute, "");
}

#define NO_API 
IMPLEMENT_OBJECT_DOC(RegistrationEngine, "RegistrationEngine", NO_API,
    "RegistrationEngine(optimizer, image_type, settings=None)\n\n\
    Args:\n\
        optimizer (str) : Name of the optimizer method to use (e.g. blocked_graph_cut') \n\
        image_type (int) : Image type of the image, see image.PixelType_* \n\
        settings (dict) : Dictionary of all settings for the engine \n\
    ");
IMPLEMENT_OBJECT_CONSTRUCTOR(RegistrationEngine, Object);

RegistrationEngine::~RegistrationEngine()
{
    delete _optimizer;
}
void RegistrationEngine::object_init()
{
    _optimizer = nullptr;
    _pyramid_level_min = 0;
    _pyramid_level_max = 6;
    _image_pair_count = 0;
    _normalize_images = true;
}
void RegistrationEngine::object_python_init(const Tuple& args, const Dict& )
{
    _optimizer = nullptr;
    _pyramid_level_min = 0;
    _pyramid_level_max = 6;
    _image_pair_count = 0;
    _normalize_images = true;

    const char* optimizer_name = 0;
    image::PixelType image_type = image::PixelType_Unknown;
    Dict settings;

    if (args.size() >= 2)
    {
        optimizer_name = args.get<const char*>(0);
        image_type = args.get<image::PixelType>(1);
    }
    else
    {
        PYTHON_ERROR(ValueError, "Expected at least two arguments");
    }
    if (args.size() >= 3)
    {
        settings = python_convert::from_python<Dict>(args.get(2));
    }

    _optimizer = create_optimizer(optimizer_name, image_type, settings);

    if (settings.has_key("pyramid_level_min"))
        _pyramid_level_min = settings.get<int>("pyramid_level_min");
    if (settings.has_key("pyramid_level_max"))
        _pyramid_level_max = settings.get<int>("pyramid_level_max");
    if (settings.has_key("normalize_images"))
        _normalize_images = settings.get<bool>("normalize_images");

}
void RegistrationEngine::set_constraints(ImageObject* values, ImageObject* mask)
{
    if (values)
    {
        if (values->pixel_type() != image::PixelType_Vec3d)
            PYTHON_ERROR(ValueError, "Constraint value image needs to be of Vec3d type!");

        _constraint_pyramid[0] = values->image();
    }
    else
    {
        _constraint_pyramid[0] = ImageVec3d();
    }
    if (mask)
    {
        if (mask->pixel_type() != image::PixelType_UInt8)
            PYTHON_ERROR(ValueError, "Constraint mask needs to be of UInt8 type!");

        _constraint_mask_pyramid[0] = mask->image();
    }
    else
    {
        _constraint_mask_pyramid[0] = ImageUInt8();
    }

}
void RegistrationEngine::set_starting_guess(ImageObject* starting_guess)
{
    if (starting_guess)
    {
        if (starting_guess->pixel_type() != image::PixelType_Vec3d)
            PYTHON_ERROR(ValueError, "Starting guess needs to be of Vec3d type!");

        _deformation_pyramid[0] = starting_guess->image();
    }
    else
    {
        _deformation_pyramid[0] = ImageVec3d();
    }
}
ImageObject* RegistrationEngine::execute(const Tuple& fixed, const Tuple& moving)
{
    _fixed_pyramid.clear();
    _moving_pyramid.clear();
    _deformation_pyramid.clear();
    _residual_pyramid.clear();
    _constraint_mask_pyramid.clear();
    _constraint_pyramid.clear();

    if (fixed.size() != moving.size())
        PYTHON_ERROR_R(ValueError, nullptr, "Expected an equal number of fixed- and moving images");
        
    _image_pair_count = (int)fixed.size();
    if (_image_pair_count == 0)
        PYTHON_ERROR_R(ValueError, nullptr, "invalid number of image pairs");

    _fixed_pyramid.resize(_pyramid_level_max);
    _moving_pyramid.resize(_pyramid_level_max);
    _deformation_pyramid.resize(_pyramid_level_max);
    _residual_pyramid.resize(_pyramid_level_max -1);
    _constraint_mask_pyramid.resize(_pyramid_level_max);
    _constraint_pyramid.resize(_pyramid_level_max);

    for (int i = 0; i < _image_pair_count; ++i)
    {
        ImageObject* fixed_img = fixed.get<ImageObject*>(i);
        ImageObject* moving_img = moving.get<ImageObject*>(i);

        int pixel_type = fixed_img->pixel_type();

        if (fixed_img->pixel_type() != moving_img->pixel_type() ||
            fixed_img->size() != moving_img->size())
            PYTHON_ERROR_R(ValueError, nullptr, "Image pairs needs to have the same size and pixel type");

        _fixed_pyramid[0].resize(_image_pair_count);
        _moving_pyramid[0].resize(_image_pair_count);

        if (_normalize_images && pixel_type == image::PixelType_Float32)
        {
            PYTHON_STDOUT("Normalizes image pair...\n");
            _fixed_pyramid[0][i] = normalize_image<ImageFloat32>(fixed_img->image());
            _moving_pyramid[0][i] = normalize_image<ImageFloat32>(moving_img->image());
        }
        else if (_normalize_images && pixel_type == image::PixelType_Float64)
        {
            PYTHON_STDOUT("Normalizes image pair...\n");
            _fixed_pyramid[0][i] = normalize_image<ImageFloat64>(fixed_img->image());
            _moving_pyramid[0][i] = normalize_image<ImageFloat64>(moving_img->image());
        }
        else
        {
            // We don't normalize non-float images
            _fixed_pyramid[0][i] = fixed_img->image();
            _moving_pyramid[0][i] = moving_img->image();
        }
    }

    // Confirm that all images are of same size

    if (_deformation_pyramid[0].valid() && _deformation_pyramid[0].size() != _fixed_pyramid[0][0].size())
        PYTHON_ERROR_R(ValueError, nullptr, "Starting guess needs to be of same size as the image pair.");

    if (_constraint_mask_pyramid[0].valid() && _constraint_mask_pyramid[0].size() != _fixed_pyramid[0][0].size())
        PYTHON_ERROR_R(ValueError, nullptr, "Constraint mask needs to be of same size as the image pair.");

    if (_constraint_pyramid[0].valid() && _constraint_pyramid[0].size() != _fixed_pyramid[0][0].size())
        PYTHON_ERROR_R(ValueError, nullptr, "Constraint value image needs to be of same size as the image pair.");


    build_pyramid();

    for (int l = _pyramid_level_max - 1; l >= 0; l--)
    {
        if (l >= _pyramid_level_min)
        {
            _optimizer->execute(&_fixed_pyramid[l][0], &_moving_pyramid[l][0], _image_pair_count, _constraint_mask_pyramid[l], _constraint_pyramid[l], _deformation_pyramid[l]);
        }

        if (l > 0) 
        {
            image::upsample_vectorfield(_deformation_pyramid[l], 2.0, _residual_pyramid[l - 1], _deformation_pyramid[l - 1]);
        }
    }
    return object_new<ImageObject>(_deformation_pyramid[0]);
}

void RegistrationEngine::build_pyramid()
{
    PYTHON_STDOUT("Building image pyramid... ");

    if (!_constraint_mask_pyramid[0].valid())
        _constraint_mask_pyramid[0] = ImageUInt8(_fixed_pyramid[0][0].ndims(), _fixed_pyramid[0][0].size(), uint8_t(0));

    for (int l = 0; l < _pyramid_level_max - 1; ++l)
    {
        _fixed_pyramid[l+1].resize(_image_pair_count);
        _moving_pyramid[l+1].resize(_image_pair_count);

        for (int i = 0; i < _image_pair_count; ++i)
        {
            double s = unit_sigma(_fixed_pyramid[l][i]);
            _fixed_pyramid[l + 1][i] = image::downsample_image_gaussian(_fixed_pyramid[l][i], 0.5, s);

            s = unit_sigma(_moving_pyramid[l][i]);
            _moving_pyramid[l + 1][i] = image::downsample_image_gaussian(_moving_pyramid[l][i], 0.5, s);
        }

        if (_constraint_mask_pyramid[l])
            _constraint_mask_pyramid[l + 1] = downsample_mask(_constraint_mask_pyramid[l], 0.5);

        if (_constraint_pyramid[l])
            _constraint_pyramid[l + 1] = downsample_constraint_values(_constraint_pyramid[l], _constraint_mask_pyramid[l], 0.5);
    }

    if (!_deformation_pyramid[0].valid()) // Has no starting guess
        _deformation_pyramid[0] = ImageVec3d(_fixed_pyramid[0][0].ndims(), _fixed_pyramid[0][0].size(), Vec3d(0, 0, 0));

    for (int l = 0; l < _pyramid_level_max -1; l++)
    {
        _deformation_pyramid[l + 1] = image::downsample_vectorfield(_deformation_pyramid[l], 0.5, _residual_pyramid[l]);
    }

    PYTHON_STDOUT("Done.\n");
}
double RegistrationEngine::unit_sigma(const Image& img)
{
    Vec3d spacing = img.spacing();
    double min_spacing = spacing.x;
    min_spacing = std::min(min_spacing, spacing.y);
    min_spacing = std::min(min_spacing, spacing.z);
    return min_spacing;
}
Optimizer* RegistrationEngine::create_optimizer(const char* name, 
    image::PixelType image_type,
    const Dict& settings) const
{
    if (strcmp(name, "blocked_graph_cut") == 0)
    {
        switch (image_type)
        {
        case image::PixelType_Float32:
            return new BlockedGraphCutOptimizer<ImageFloat32>(settings);
        case image::PixelType_Float64:
            return new BlockedGraphCutOptimizer<ImageFloat64>(settings);
        case image::PixelType_Vec4f:
            return new BlockedGraphCutOptimizer<ImageColorf>(settings);
        default:
            PYTHON_ERROR_R(NotImplementedError, nullptr, "Image type %s not implemented", image::pixel_type_to_string(image_type));
        }
    }
    PYTHON_ERROR_R(ValueError, nullptr, "No optimizer with name '%s' found", name);
}