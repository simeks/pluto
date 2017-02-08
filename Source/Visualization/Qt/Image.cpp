#include <Core/Common.h>

#include "Image.h"

#include <Core/Image/Image.h>

NumpyArray format_data(const NumpyArray& img, visualization::ImageType image_type)
{
    NumpyArray out = img.contiguous();
    out = out.cast(NPY_UINT8);

    if (image_type == visualization::ImageType_Bool)
    {
        uint8_t* data = (uint8_t*)out.data();

        // Assumes 2d image
        for (int y = 0; y < out.shape()[0]; ++y)
        {
            for (int x = 0; x < out.shape()[1]; ++x)
            {
                if (data[y*out.stride(0) + x*out.stride(1)] > 0)
                    data[y*out.stride(0) + x*out.stride(1)] = 255;
                else
                    data[y*out.stride(0) + x*out.stride(1)] = 0;
            }
        }
    }
    return out;
}

QImage convert_to_qimage(const NumpyArray& img)
{
    QImage::Format fmt = QImage::Format_Invalid;

    if (img.ndims() == 3)
    {
        // 2D image with multiple channels
        int n_channels = img.shape()[2];
        if (n_channels == 3)
        {
            fmt = QImage::Format_RGB888;
        }
        else if (n_channels == 4)
        {
            fmt = QImage::Format_RGBA8888;
        }
    }
    else if (img.ndims() == 2 || img.ndims() == 1)
    {
        fmt = QImage::Format_Grayscale8;
    }

    if (fmt == QImage::Format_Invalid)
    {
        return QImage();
    }

    return QImage((uint8_t*)img.data(), img.shape()[1], img.shape()[0], (int)img.strides()[0], fmt);
}

