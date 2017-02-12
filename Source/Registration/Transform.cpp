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
        Image image_to_transform = image;
        Image result;

        assert(deformation.size() == image.size());

        // @hack only 3d registration is supported currently so to support 2d images
        // we reshape them to 3d images with z axis set to length 1
        bool is_2d = image_to_transform.ndims() == 2;
        if (is_2d)
            image_to_transform = image_to_transform.reshape(3, Vec3i(image_to_transform.size().x, image_to_transform.size().y, 1));
            
        if (image.pixel_type() == image::PixelType_UInt8)
        {
            result = transform::transform_image<ImageUInt8>(image_to_transform, deformation);
        }
        else if (image.pixel_type() == image::PixelType_UInt16)
        {
            result = transform::transform_image<ImageUInt16>(image_to_transform, deformation);
        }
        else if (image.pixel_type() == image::PixelType_UInt32)
        {
            result = transform::transform_image<ImageUInt32>(image_to_transform, deformation);
        }
        else if (image.pixel_type() == image::PixelType_Float32)
        {
            result = transform::transform_image<ImageFloat32>(image_to_transform, deformation);
        }
        else if (image.pixel_type() == image::PixelType_Float64)
        {
            result = transform::transform_image<ImageFloat64>(image_to_transform, deformation);
        }
        else if (image.pixel_type() == image::PixelType_Vec3u8)
        {
            result = transform::transform_image<ImageVec3u8>(image_to_transform, deformation);
        }
        else if (image.pixel_type() == image::PixelType_Vec3f)
        {
            result = transform::transform_image<ImageVec3f>(image_to_transform, deformation);
        }
        else if (image.pixel_type() == image::PixelType_Vec4u8)
        {
            result = transform::transform_image<ImageColorf>(image_to_transform, deformation);
        }
        else if (image.pixel_type() == image::PixelType_Vec4f)
        {
            result = transform::transform_image<ImageColorf>(image_to_transform, deformation);
        }
        else
        {
            PYTHON_ERROR_R(ValueError, Image(), "Unsupported image format (%s)", image::pixel_type_to_string(image_to_transform.pixel_type()));
        }

        // Return image to its original format
        if (is_2d)
            result = result.reshape(2, Vec3i(result.size().x, result.size().y, 1));

        return result;
    }
    PYTHON_ERROR_R(ValueError, Image(), "Missing arguments");
}
