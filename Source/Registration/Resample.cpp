#include <Core/Common.h>

#include <Core/Image/Image.h>
#include "GaussianFilter.h"
#include "Resample.h"

template<typename TImage>
static Image downsample_image_tpl(const TImage& img, double scale)
{
    assert(scale > 0.0 && scale < 1.0);
    double inv_scale = 1.0 / scale;

    Vec3i dims(1, 1, 1);
    for (int i = 0; i < img.ndims(); ++i)
    {
        dims[i] = (int)ceil(img.size()[i] * scale);
    }

    TImage result(img.ndims(), dims);
    
    Vec3d spacing(1,1,1);
    for (int i = 0; i < img.ndims(); ++i)
    {
        spacing[i] = img.spacing()[i] * inv_scale;
    }
    result.set_spacing(spacing);
    result.set_origin(img.origin());

#pragma omp parallel for
    for (int z = 0; z < dims.z; ++z)
    {
        for (int y = 0; y < dims.y; ++y)
        {
            for (int x = 0; x < dims.x; ++x)
            {
                result(x, y, z) = img(Vec3i(x, y, z) * inv_scale);
            }
        }
    }
    return result;
}
Image image::downsample_image(const Image& img, double scale)
{
    assert(img.valid());

    switch (img.pixel_type())
    {
    case image::PixelType_UInt8:
        return downsample_image_tpl<ImageUInt8>(img, scale);
    case image::PixelType_UInt16:
        return downsample_image_tpl<ImageUInt16>(img, scale);
    case image::PixelType_UInt32:
        return downsample_image_tpl<ImageUInt32>(img, scale);
    case image::PixelType_Float32:
        return downsample_image_tpl<ImageFloat32>(img, scale);
    case image::PixelType_Float64:
        return downsample_image_tpl<ImageFloat64>(img, scale);
    case image::PixelType_Vec3u8:
        return downsample_image_tpl<ImageVec3u8>(img, scale);
    case image::PixelType_Vec3f:
        return downsample_image_tpl<ImageVec3f>(img, scale);
    case image::PixelType_Vec3d:
        return downsample_image_tpl<ImageVec3d>(img, scale);
    case image::PixelType_Vec4u8:
        return downsample_image_tpl<ImageRGBA32>(img, scale);
    case image::PixelType_Vec4f:
        return downsample_image_tpl<ImageColorf>(img, scale);
    }
    return Image();
}
Image image::downsample_image_gaussian(const Image& img, double scale, double sigma)
{
    Image filtered = image::gaussian_filter(img, sigma);
    return downsample_image(filtered, scale);
}

ImageVec3d image::downsample_vectorfield(const ImageVec3d& field, double scale, ImageVec3d& residual)
{
    assert(scale > 0.0 && scale < 1.0);
    double inv_scale = 1.0 / scale;

    Vec3i old_dims = field.size();
    Vec3i new_dims(1, 1, 1);
    Vec3d spacing(1, 1, 1);
    for (int i = 0; i < field.ndims(); ++i)
    {
        new_dims[i] = (int)ceil(old_dims[i] * scale);
        spacing[i] = field.spacing()[i] * inv_scale;
    }

    ImageVec3d result(field.ndims(), new_dims, Vec3d(0, 0, 0));
    result.set_spacing(spacing);
    result.set_origin(field.origin());

#pragma omp parallel for
    for (int z = 0; z < new_dims.z; ++z)
    {
        for (int y = 0; y < new_dims.y; ++y)
        {
            for (int x = 0; x < new_dims.x; ++x)
            {
                Vec3i p = Vec3i(x, y, z) * inv_scale;
                Vec3d v = field(p);
                v += field.at(p + Vec3i(1, 0, 0), image::Border_Replicate);
                v += field.at(p + Vec3i(0, 1, 0), image::Border_Replicate);
                v += field.at(p + Vec3i(0, 0, 1), image::Border_Replicate);
                v += field.at(p + Vec3i(1, 1, 0), image::Border_Replicate);
                v += field.at(p + Vec3i(1, 0, 1), image::Border_Replicate);
                v += field.at(p + Vec3i(0, 1, 1), image::Border_Replicate);
                v += field.at(p + Vec3i(1, 1, 1), image::Border_Replicate);

                result(x, y, z) = scale * v / 8.0;
            }
        }
    }

    residual = field.clone();

#pragma omp parallel for
    for (int z = 0; z < old_dims.z; ++z)
    {
        for (int y = 0; y < old_dims.y; ++y)
        {
            for (int x = 0; x < old_dims.x; ++x)
            {
                residual(x, y, z) = field(x, y, z) - inv_scale * result.linear_at(scale*Vec3d(x, y, z), image::Border_Replicate);
            }
        }
    }
    return result;
}

ImageVec3d image::upsample_vectorfield(const ImageVec3d& field, double scale)
{
    assert(scale > 1.0);
    double inv_scale = 1.0 / scale;

    Vec3i dims(1, 1, 1);
    Vec3d spacing(1, 1, 1);
    for (int i = 0; i < field.ndims(); ++i)
    {
        dims[i] = (int)floor(field.size()[i] * scale);
        spacing[i] = field.spacing()[i] * inv_scale;
    }

    ImageVec3d result(field.ndims(), dims);
    result.set_spacing(spacing);
    result.set_origin(field.origin());

#pragma omp parallel for
    for (long z = 0; z < dims.z; z++)
    {
        for (long y = 0; y < dims.y; y++)
        {
            for (long x = 0; x < dims.x; x++)
            {
                result(x, y, z) = scale * field.linear_at(inv_scale*Vec3d(x, y, z));
            }
        }
    }
    return result;
}
void image::upsample_vectorfield(const ImageVec3d& field, double scale, const ImageVec3d& residual, ImageVec3d& out)
{
    assert(scale > 1.0);
    double inv_scale = 1.0 / scale;

    Vec3i dims = out.size();
    if (residual.valid())
    {
#pragma omp parallel for
        for (long z = 0; z < dims.z; z++)
        {
            for (long y = 0; y < dims.y; y++)
            {
                for (long x = 0; x < dims.x; x++)
                {
                    out(x, y, z) = scale * field.linear_at(inv_scale*Vec3d(x, y, z)) + residual(x, y, z);
                }
            }
        }
    }
    else
    {
#pragma omp parallel for
        for (long z = 0; z < dims.z; z++)
        {
            for (long y = 0; y < dims.y; y++)
            {
                for (long x = 0; x < dims.x; x++)
                {
                    out(x, y, z) = scale * field.linear_at(inv_scale*Vec3d(x, y, z));
                }
            }
        }
    }
}
