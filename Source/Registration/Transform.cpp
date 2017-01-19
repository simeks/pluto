#include <Core/Common.h>

#include <Flow/FlowContext.h>
#include <Core/Image/Image.h>

#include "Transform.h"

namespace transform
{
    template<typename TImage>
    TImage transform_image(const TImage& source, const ImageVec3d& deformation)
    {
        assert(source.size() == deformation.size());

        TImage result(source.ndims(), source.size());
        result.set_spacing(source.spacing());
        result.set_origin(source.origin());
        Vec3i size = result.size();
#pragma omp parallel for
        for (int z = 0; z < size.z; ++z)
        {
            for (int y = 0; y < size.y; ++y)
            {
                for (int x = 0; x < size.x; ++x)
                {
                    result(x, y, z) = source.linear_at(Vec3d(x, y, z) + deformation(x, y, z));
                }
            }
        }
        return result;
    }
}

Image transform::transform(const Image& image, const Image& deformation)
{
    if (image.valid() && deformation.valid())
    {
        assert(deformation.size() == image.size());

        if (image.pixel_type() == image::PixelType_UInt8)
        {
            return transform::transform_image<ImageUInt8>(image, deformation);
        }
        else if (image.pixel_type() == image::PixelType_UInt16)
        {
            return transform::transform_image<ImageUInt16>(image, deformation);
        }
        else if (image.pixel_type() == image::PixelType_UInt32)
        {
            return transform::transform_image<ImageUInt32>(image, deformation);
        }
        else if (image.pixel_type() == image::PixelType_Float32)
        {
            return transform::transform_image<ImageFloat32>(image, deformation);
        }
        else if (image.pixel_type() == image::PixelType_Float64)
        {
            return transform::transform_image<ImageFloat64>(image, deformation);
        }
        else if (image.pixel_type() == image::PixelType_Vec3u8)
        {
            return transform::transform_image<ImageVec3u8>(image, deformation);
        }
        else if (image.pixel_type() == image::PixelType_Vec3f)
        {
            return transform::transform_image<ImageVec3f>(image, deformation);
        }
        else if (image.pixel_type() == image::PixelType_Vec4u8)
        {
            return transform::transform_image<ImageColorf>(image, deformation);
        }
        else if (image.pixel_type() == image::PixelType_Vec4f)
        {
            return transform::transform_image<ImageColorf>(image, deformation);
        }
        else
        {
            PYTHON_ERROR_R(ValueError, Image(), "Unsupported image format (%s)", image::pixel_type_to_string(image.pixel_type()));
        }
    }
    PYTHON_ERROR_R(ValueError, Image(), "Missing arguments");
}
