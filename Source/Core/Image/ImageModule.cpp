#include "Common.h"

#include "Image.h"
#include "ImageModule.h"
#include "Python/PythonFunction.h"
#include "Types.h"

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(ImageModule, type_to_string, image::PixelType);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(ImageModule, type_from_string, const char*);

ImageModule::ImageModule()
{
}
ImageModule::~ImageModule()
{
}
void ImageModule::post_init()
{
    MODULE_ADD_PYTHON_FUNCTION(ImageModule, type_to_string, "type_to_string(type)");
    MODULE_ADD_PYTHON_FUNCTION(ImageModule, type_from_string, "type_from_string(type)");

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
const char* ImageModule::type_to_string(image::PixelType type)
{
    return image::pixel_type_to_string(type);
}
image::PixelType ImageModule::type_from_string(const char* str)
{
    return (image::PixelType)image::string_to_pixel_type(str);
}
const char* ImageModule::name()
{
    return "image_api";
}
