#include "Common.h"

#include "Image.h"
#include "ImageModule.h"
#include "Python/Function.h"
#include "Python/Module.h"
#include "Types.h"


const char* image::type_to_string(image::PixelType type)
{
    return image::pixel_type_to_string(type);
}
image::PixelType image::type_from_string(const char* str)
{
    return (image::PixelType)image::string_to_pixel_type(str);
}

namespace py = python;

PYTHON_MODULE(image_api)
{
    py::def(module, "type_to_string", &image::type_to_string, "type_to_string(type)");
    py::def(module, "type_from_string", &image::type_from_string, "type_from_string(type)");

    py::def(module, "PixelType_Unknown", image::PixelType_Unknown);
    py::def(module, "PixelType_UInt8", image::PixelType_UInt8);
    py::def(module, "PixelType_Int8", image::PixelType_Int8);
    py::def(module, "PixelType_UInt16", image::PixelType_UInt16);
    py::def(module, "PixelType_Int16", image::PixelType_Int16);
    py::def(module, "PixelType_UInt32", image::PixelType_UInt32);
    py::def(module, "PixelType_Int32", image::PixelType_Int32);
    py::def(module, "PixelType_Float32", image::PixelType_Float32);
    py::def(module, "PixelType_Float64", image::PixelType_Float64);
    py::def(module, "PixelType_Vec3u8", image::PixelType_Vec3u8);
    py::def(module, "PixelType_Vec3f", image::PixelType_Vec3f);
    py::def(module, "PixelType_Vec3d", image::PixelType_Vec3d);
    py::def(module, "PixelType_Vec4u8", image::PixelType_Vec4u8);
    py::def(module, "PixelType_Vec4f", image::PixelType_Vec4f);
    py::def(module, "PixelType_Vec4d", image::PixelType_Vec4d);
}

void image::install_python_module()
{
    PYTHON_MODULE_INSTALL(image_api);
}
