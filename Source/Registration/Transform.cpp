#include <Core/Common.h>

#include <Flow/FlowContext.h>
#include <Core/Image/ImageObject.h>
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

ImageObject* transform::transform(ImageObject* image_obj, ImageObject* deformation_obj)
{
    if (image_obj && deformation_obj)
    {
        ImageVec3d deformation = deformation_obj->image();
        assert(deformation.size() == image_obj->size());

        ImageObject* result = nullptr;
        if (image_obj->pixel_type() == image::PixelType_UInt8)
        {
            // Convert image to float32, transform, then convert back
            ImageUInt8 img = transform::transform_image<ImageFloat32>(image_obj->image(), deformation);
            result = object_new<ImageObject>(img);
        }
        else if (image_obj->pixel_type() == image::PixelType_Float32)
        {
            Image img = transform::transform_image<ImageFloat32>(image_obj->image(), deformation);
            result = object_new<ImageObject>(img);
        }
        else if (image_obj->pixel_type() == image::PixelType_Float64)
        {
            Image img = transform::transform_image<ImageFloat64>(image_obj->image(), deformation);
            result = object_new<ImageObject>(img);
        }
        else if (image_obj->pixel_type() == image::PixelType_Vec3u8)
        {
            Image img = transform::transform_image<ImageVec3u8>(image_obj->image(), deformation);
            result = object_new<ImageObject>(img);
        }
        else if (image_obj->pixel_type() == image::PixelType_Vec3f)
        {
            Image img = transform::transform_image<ImageVec3u8>(image_obj->image(), deformation);
            result = object_new<ImageObject>(img);
        }
        else if (image_obj->pixel_type() == image::PixelType_Vec4u8)
        {
            ImageRGBA32 img = transform::transform_image<ImageColorf>(image_obj->image(), deformation);
            result = object_new<ImageObject>(img);
        }
        else if (image_obj->pixel_type() == image::PixelType_Vec4f)
        {
            Image img = transform::transform_image<ImageColorf>(image_obj->image(), deformation);
            result = object_new<ImageObject>(img);
        }
        else
        {
            PYTHON_ERROR_R(ValueError, nullptr, "Unsupported image format (%s)", image::pixel_type_to_string(image_obj->pixel_type()));
        }
        return result;
    }
    return nullptr;
}
