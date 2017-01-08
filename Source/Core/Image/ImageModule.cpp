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
    set_object("PixelType_Unknown", python_convert::to_python<int>(image::PixelType_Unknown));
    set_object("PixelType_UInt8", python_convert::to_python<int>(image::PixelType_UInt8));
    set_object("PixelType_Int8", python_convert::to_python<int>(image::PixelType_Int8));
    set_object("PixelType_UInt16", python_convert::to_python<int>(image::PixelType_UInt16));
    set_object("PixelType_Int16", python_convert::to_python<int>(image::PixelType_Int16));
    set_object("PixelType_UInt32", python_convert::to_python<int>(image::PixelType_UInt32));
    set_object("PixelType_Int32", python_convert::to_python<int>(image::PixelType_Int32));
    set_object("PixelType_Float32", python_convert::to_python<int>(image::PixelType_Float32));
    set_object("PixelType_Float64", python_convert::to_python<int>(image::PixelType_Float64));
    set_object("PixelType_Vec3u8", python_convert::to_python<int>(image::PixelType_Vec3u8));
    set_object("PixelType_Vec3f", python_convert::to_python<int>(image::PixelType_Vec3f));
    set_object("PixelType_Vec3d", python_convert::to_python<int>(image::PixelType_Vec3d));
    set_object("PixelType_Vec4u8", python_convert::to_python<int>(image::PixelType_Vec4u8));
    set_object("PixelType_Vec4f", python_convert::to_python<int>(image::PixelType_Vec4f));
    set_object("PixelType_Vec4d", python_convert::to_python<int>(image::PixelType_Vec4d));
}
const char* ImageModule::name()
{
    return "image";
}
