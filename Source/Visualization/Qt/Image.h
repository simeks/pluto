#ifndef __VISUALIZATION_QT_IMAGE_H__
#define __VISUALIZATION_QT_IMAGE_H__

#include <QImage>

#include <Core/Image/Vec3.h>

class NumpyArray;

namespace visualization
{
    enum ImageType
    {
        ImageType_Unknown,
        ImageType_Color,
        ImageType_GrayScale,
        ImageType_Bool
    };

};

NumpyArray format_data(const NumpyArray& img, visualization::ImageType image_type);

QImage convert_to_qimage(const NumpyArray& img, const Vec3d& spacing=Vec3d(1,1,1));

#endif // __VISUALIZATION_QT_IMAGE_H__
