#ifndef __CORE_IMAGE_RESAMPLE_H__
#define __CORE_IMAGE_RESAMPLE_H__

namespace image
{
    Image downsample_image(const Image& img, double scale);
    Image downsample_image_gaussian(const Image& img, double scale, double sigma);

    ImageVec3d downsample_vectorfield(const ImageVec3d& field, double scale, ImageVec3d& residual);

    ImageVec3d upsample_vectorfield(const ImageVec3d& field, double scale);
    void upsample_vectorfield(const ImageVec3d& field, double scale, const ImageVec3d& residual, ImageVec3d& out);
}

#endif // __CORE_IMAGE_RESAMPLE_H__
