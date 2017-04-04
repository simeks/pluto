#include <Core/Common.h>
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
            PYTHON_ERROR(PyExc_TypeError, "Only float and double supported! (Type: %s)", image::pixel_type_to_string(img.pixel_type()));
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
    Vec3d new_spacing(1, 1, 1);
    for (int i = 0; i < mask.ndims(); ++i)
    {
        new_dims[i] = (int)ceil(old_dims[i] * scale);
        new_spacing[i] = mask.spacing()[i] * inv_scale;
    }

    ImageUInt8 result(mask.ndims(), new_dims, (uint8_t)0);
    result.set_spacing(new_spacing);
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
    Vec3d new_spacing(1, 1, 1);
    for (int i = 0; i < values.ndims(); ++i)
    {
        new_dims[i] = (int)ceil(old_dims[i] * scale);
        new_spacing[i] = mask.spacing()[i] * inv_scale;
    }

    ImageVec3d result(values.ndims(), new_dims, Vec3d(0, 0, 0));
    result.set_spacing(new_spacing);
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


RegistrationEngine::RegistrationEngine() :
    _optimizer(nullptr),
    _image_type(image::PixelType_Unknown),
    _pyramid_level_min(0),
    _pyramid_level_max(6),
    _image_pair_count(0),
    _normalize_images(true)
{
}
RegistrationEngine::RegistrationEngine(const Tuple& args) :
    _optimizer(nullptr),
    _image_type(image::PixelType_Unknown),
    _pyramid_level_min(0),
    _pyramid_level_max(6),
    _image_pair_count(0),
    _normalize_images(true)
{
    const char* optimizer_name = 0;
    _image_type = image::PixelType_Unknown;
    Dict settings;

    if (args.size() >= 2)
    {
        optimizer_name = args.get<const char*>(0);
        _image_type = args.get<image::PixelType>(1);
    }
    else
    {
        PYTHON_ERROR(PyExc_ValueError, "Expected at least two arguments");
    }
    if (args.size() >= 3)
    {
        settings = python::from_python<Dict>(args.get(2));
    }

    _optimizer = create_optimizer(optimizer_name, _image_type, settings);

    if (settings.has_key("pyramid_level_min"))
        _pyramid_level_min = settings.get<int>("pyramid_level_min");
    if (settings.has_key("pyramid_level_max"))
        _pyramid_level_max = settings.get<int>("pyramid_level_max");
    if (settings.has_key("normalize_images"))
        _normalize_images = settings.get<bool>("normalize_images");

}
RegistrationEngine::~RegistrationEngine()
{
    delete _optimizer;
}
void RegistrationEngine::set_constraints(const Image& values, const Image& mask)
{
    if (values)
    {
        if (values.pixel_type() != image::PixelType_Vec3d)
            PYTHON_ERROR(PyExc_ValueError, "Constraint value image needs to be of Vec3d type!");

        _constraint_pyramid[0] = values;
    }
    else
    {
        _constraint_pyramid[0] = ImageVec3d();
    }
    if (mask)
    {
        if (mask.pixel_type() != image::PixelType_UInt8)
            PYTHON_ERROR(PyExc_ValueError, "Constraint mask needs to be of UInt8 type!");

        _constraint_mask_pyramid[0] = mask;
    }
    else
    {
        _constraint_mask_pyramid[0] = ImageUInt8();
    }

}
void RegistrationEngine::set_starting_guess(const Image& starting_guess)
{
    if (starting_guess)
    {
        if (starting_guess.pixel_type() != image::PixelType_Vec3d)
            PYTHON_ERROR(PyExc_ValueError, "Starting guess needs to be of Vec3d type!");

        _deformation_pyramid[0] = starting_guess;
    }
    else
    {
        _deformation_pyramid[0] = ImageVec3d();
    }
}
Image RegistrationEngine::execute(const Tuple& fixed, const Tuple& moving)
{
    _fixed_pyramid.clear();
    _moving_pyramid.clear();
    _deformation_pyramid.clear();
    _residual_pyramid.clear();
    _constraint_mask_pyramid.clear();
    _constraint_pyramid.clear();

    if (fixed.size() != moving.size())
        PYTHON_ERROR(PyExc_ValueError, "Expected an equal number of fixed- and moving images");
        
    _image_pair_count = (int)fixed.size();
    if (_image_pair_count == 0)
        PYTHON_ERROR(PyExc_ValueError, "invalid number of image pairs");

    _fixed_pyramid.resize(_pyramid_level_max);
    _moving_pyramid.resize(_pyramid_level_max);
    _deformation_pyramid.resize(_pyramid_level_max);
    _residual_pyramid.resize(_pyramid_level_max -1);
    _constraint_mask_pyramid.resize(_pyramid_level_max);
    _constraint_pyramid.resize(_pyramid_level_max);

    _fixed_pyramid[0].resize(_image_pair_count);
    _moving_pyramid[0].resize(_image_pair_count);

    for (int i = 0; i < _image_pair_count; ++i)
    {
        Image fixed_img = fixed.get<Image>(i);
        Image moving_img = moving.get<Image>(i);
        if (!fixed_img || !moving_img)
            PYTHON_ERROR(PyExc_ValueError, "Missing image");

        if (fixed_img.pixel_type() != moving_img.pixel_type() ||
            fixed_img.size() != moving_img.size())
            PYTHON_ERROR(PyExc_ValueError, "Image pairs needs to have the same size and pixel type");

        /// @Hack until we have a dedicated 2d registration engine. We just reshape 2d images to 3d with the z axis set to length 1
        if (fixed_img.ndims() == 2)
        {
            fixed_img = fixed_img.reshape(3, Vec3i(fixed_img.size().x, fixed_img.size().y, 1));
        }
        if (moving_img.ndims() == 2)
        {
            moving_img = moving_img.reshape(3, Vec3i(moving_img.size().x, moving_img.size().y, 1));
        }

        if (_image_type == image::PixelType_Float32)
        {
            if (_normalize_images)
            {
                _fixed_pyramid[0][i] = normalize_image<ImageFloat32>(fixed_img);
                _moving_pyramid[0][i] = normalize_image<ImageFloat32>(moving_img);
            }
            else
            {
                _fixed_pyramid[0][i] = ImageFloat32(fixed_img);
                _moving_pyramid[0][i] = ImageFloat32(moving_img);
            }
        }
        else if (_image_type == image::PixelType_Float64)
        {
            if (_normalize_images)
            {
                _fixed_pyramid[0][i] = normalize_image<ImageFloat64>(fixed_img);
                _moving_pyramid[0][i] = normalize_image<ImageFloat64>(moving_img);
            }
            else
            {
                _fixed_pyramid[0][i] = ImageFloat64(fixed_img);
                _moving_pyramid[0][i] = ImageFloat64(moving_img);
            }
        }
        else
        {
            // We don't normalize non-float images
            _fixed_pyramid[0][i] = fixed_img;
            _moving_pyramid[0][i] = moving_img;
        }
    }

    // Confirm that all images are of same size

    if (_deformation_pyramid[0].valid() && _deformation_pyramid[0].size() != _fixed_pyramid[0][0].size())
        PYTHON_ERROR(PyExc_ValueError, "Starting guess needs to be of same size as the image pair.");

    if (_constraint_mask_pyramid[0].valid() && _constraint_mask_pyramid[0].size() != _fixed_pyramid[0][0].size())
        PYTHON_ERROR(PyExc_ValueError, "Constraint mask needs to be of same size as the image pair.");

    if (_constraint_pyramid[0].valid() && _constraint_pyramid[0].size() != _fixed_pyramid[0][0].size())
        PYTHON_ERROR(PyExc_ValueError, "Constraint value image needs to be of same size as the image pair.");


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
    return _deformation_pyramid[0];
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
    
    if (spacing.z > 0) // 0 spacing makes no sense
        min_spacing = std::min(min_spacing, spacing.z);

    return min_spacing;
}
Optimizer* RegistrationEngine::create_optimizer(const char* name, 
    image::PixelType image_type,
    const Dict& settings) const
{
    if (strcmp(name, "blocked_graph_cut") == 0 || strcmp(name, "basic_gc_deformable") == 0)
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
            PYTHON_ERROR(PyExc_NotImplementedError, "Image type %s not implemented", image::pixel_type_to_string(image_type));
        }
    }
    PYTHON_ERROR(PyExc_ValueError, "No optimizer with name '%s' found", name);
}
python::Object RegistrationEngine::python_class()
{
    static python::Class cls;
    if (cls.ptr() == Py_None)
    {
        cls = python::make_class<RegistrationEngine>("RegistrationEngine",
            "RegistrationEngine(optimizer, image_type, settings=None)\n\n\
                Args:\n\
                optimizer (str) : Name of the optimizer method to use (e.g. blocked_graph_cut') \n\
                image_type (int) : Image type of the image, see image.PixelType_* \n\
                settings (dict) : Dictionary of all settings for the engine \n\
        ");
        cls.def_init_varargs<RegistrationEngine>();
        cls.def("set_constraints", &RegistrationEngine::set_constraints,
                "set_constraints(values, mask=None)\n"
                "--\n"
                "Args:\n"
                "    value (Image) : Image of Vec3d containing displacement vectors\n"
                "    mask (Image) : Binary image specifying which voxels to constrain\n"
                "");
        cls.def("set_starting_guess", &RegistrationEngine::set_starting_guess,
                "set_starting_guess(values)\n"
                "--\n"
                "Args:\n"
                "    value (Image) : Image of Vec3d with the initial deformation field\n"
                "");
        cls.def("execute", &RegistrationEngine::execute, "");
    }
    return cls;
}
