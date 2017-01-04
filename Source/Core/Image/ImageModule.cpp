#include "Common.h"

#include "ImageObject.h"
#include "ImageModule.h"


ImageModule::ImageModule()
{
}
ImageModule::~ImageModule()
{
}
void ImageModule::post_init()
{
    add_type("Image", ImageObject::static_class());
    add_object("PixelType_Unknown", python_convert::to_python<int>(image::PixelType_Unknown));
    add_object("PixelType_UInt8", python_convert::to_python<int>(image::PixelType_UInt8));
    add_object("PixelType_UInt16", python_convert::to_python<int>(image::PixelType_UInt16));
    add_object("PixelType_UInt32", python_convert::to_python<int>(image::PixelType_UInt32));
    add_object("PixelType_Float32", python_convert::to_python<int>(image::PixelType_Float32));
    add_object("PixelType_Float64", python_convert::to_python<int>(image::PixelType_Float64));
    add_object("PixelType_Vec3u8", python_convert::to_python<int>(image::PixelType_Vec3u8));
    add_object("PixelType_Vec3f", python_convert::to_python<int>(image::PixelType_Vec3f));
    add_object("PixelType_Vec3d", python_convert::to_python<int>(image::PixelType_Vec3d));
    add_object("PixelType_Vec4u8", python_convert::to_python<int>(image::PixelType_Vec4u8));
    add_object("PixelType_Vec4f", python_convert::to_python<int>(image::PixelType_Vec4f));
    add_object("PixelType_Vec4d", python_convert::to_python<int>(image::PixelType_Vec4d));
}
const char* ImageModule::name()
{
    return "image";
}
