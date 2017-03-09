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

PYTHON_MODULE(_image)
{
    module.def("type_to_string", &image::type_to_string, "type_to_string(type)");
    module.def("type_from_string", &image::type_from_string, "type_from_string(type)");

    module.def("PixelType_Unknown", image::PixelType_Unknown);
    module.def("PixelType_UInt8", image::PixelType_UInt8);
    module.def("PixelType_Int8", image::PixelType_Int8);
    module.def("PixelType_UInt16", image::PixelType_UInt16);
    module.def("PixelType_Int16", image::PixelType_Int16);
    module.def("PixelType_UInt32", image::PixelType_UInt32);
    module.def("PixelType_Int32", image::PixelType_Int32);
    module.def("PixelType_Float32", image::PixelType_Float32);
    module.def("PixelType_Float64", image::PixelType_Float64);
    module.def("PixelType_Vec3u8", image::PixelType_Vec3u8);
    module.def("PixelType_Vec3f", image::PixelType_Vec3f);
    module.def("PixelType_Vec3d", image::PixelType_Vec3d);
    module.def("PixelType_Vec4u8", image::PixelType_Vec4u8);
    module.def("PixelType_Vec4f", image::PixelType_Vec4f);
    module.def("PixelType_Vec4d", image::PixelType_Vec4d);
}

void image::install_python_module()
{
    PYTHON_MODULE_INSTALL(_image);
}
