#include "Common.h"

#include "Python/Convert.h"
#include "Python/PythonCommon.h"
#include "Types.h"
#include "Vec3.h"

size_t image::pixel_size(int type)
{
    switch (type)
    {
    case PixelType_UInt8:
        return sizeof(uint8_t);
    case PixelType_Int8:
        return sizeof(int8_t);
    case PixelType_UInt16:
        return sizeof(uint16_t);
    case PixelType_Int16:
        return sizeof(int16_t);
    case PixelType_UInt32:
        return sizeof(uint32_t);
    case PixelType_Int32:
        return sizeof(int32_t);
    case PixelType_Float32:
        return sizeof(float);
    case PixelType_Float64:
        return sizeof(double);
    case PixelType_Vec3u8:
        return sizeof(Vec3u8);
    case PixelType_Vec3f:
        return sizeof(Vec3f);
    case PixelType_Vec3d:
        return sizeof(Vec3d);
    case PixelType_Vec4u8:
        return sizeof(uint8_t)*4;
    case PixelType_Vec4f:
        return sizeof(float) * 4;
    case PixelType_Vec4d:
        return sizeof(double) * 4;
    }
    return 0;
}

int image::string_to_pixel_type(const char* str)
{
    int pixel_t = image::PixelType_Unknown;
    if (strcmp(str, "uint8") == 0)
    {
        pixel_t = image::PixelType_UInt8;
    }
    else if (strcmp(str, "int8") == 0)
    {
        pixel_t = image::PixelType_Int8;
    }
    else if (strcmp(str, "uint16") == 0)
    {
        pixel_t = image::PixelType_UInt16;
    }
    else if (strcmp(str, "int16") == 0)
    {
        pixel_t = image::PixelType_Int16;
    }
    else if (strcmp(str, "uint32") == 0)
    {
        pixel_t = image::PixelType_UInt32;
    }
    else if (strcmp(str, "int32") == 0)
    {
        pixel_t = image::PixelType_Int32;
    }
    else if (strcmp(str, "float32") == 0)
    {
        pixel_t = image::PixelType_Float32;
    }
    else if (strcmp(str, "float64") == 0)
    {
        pixel_t = image::PixelType_Float64;
    }
    else if (strcmp(str, "vec3u8") == 0)
    {
        pixel_t = image::PixelType_Vec3u8;
    }
    else if (strcmp(str, "vec3f") == 0)
    {
        pixel_t = image::PixelType_Vec3f;
    }
    else if (strcmp(str, "vec3d") == 0)
    {
        pixel_t = image::PixelType_Vec3d;
    }
    else if (strcmp(str, "vec4u8") == 0)
    {
        pixel_t = image::PixelType_Vec4u8;
    }
    else if (strcmp(str, "vec4f") == 0)
    {
        pixel_t = image::PixelType_Vec4f;
    }
    else if (strcmp(str, "vec4d") == 0)
    {
        pixel_t = image::PixelType_Vec4d;
    }

    return pixel_t;
}
const char* image::pixel_type_to_string(int type)
{
    switch (type)
    {
    case image::PixelType_UInt8:
        return "uint8";
    case image::PixelType_Int8:
        return "int8";
    case image::PixelType_UInt16:
        return "uint16";
    case image::PixelType_Int16:
        return "int16";
    case image::PixelType_UInt32:
        return "uint32";
    case image::PixelType_Int32:
        return "int32";
    case image::PixelType_Float32:
        return "float32";
    case image::PixelType_Float64:
        return "float64";
    case image::PixelType_Vec3u8:
        return "vec3u8";
    case image::PixelType_Vec3f:
        return "vec3f";
    case image::PixelType_Vec3d:
        return "vec3d";
    case image::PixelType_Vec4u8:
        return "vec4u8";
    case image::PixelType_Vec4f:
        return "vec4f";
    case image::PixelType_Vec4d:
        return "vec4d";
    }

    return "unknown";
}

int image::num_components(int type)
{
    switch (type)
    {
    case image::PixelType_UInt8:
    case image::PixelType_Int8:
    case image::PixelType_UInt16:
    case image::PixelType_Int16:
    case image::PixelType_UInt32:
    case image::PixelType_Int32:
    case image::PixelType_Float32:
    case image::PixelType_Float64:
        return 1;
    case image::PixelType_Vec3u8:
    case image::PixelType_Vec3f:
    case image::PixelType_Vec3d:
        return 3;
    case image::PixelType_Vec4u8:
    case image::PixelType_Vec4f:
    case image::PixelType_Vec4d:
        return 4;
    }
    return -1;
}
size_t image::component_size(int type)
{
    switch (type)
    {
    case image::PixelType_UInt8:
    case image::PixelType_Int8:
    case image::PixelType_Vec3u8:
    case image::PixelType_Vec4u8:
        return 1;

    case image::PixelType_UInt16:
    case image::PixelType_Int16:
        return 2;

    case image::PixelType_UInt32:
    case image::PixelType_Int32:
    case image::PixelType_Float32:
    case image::PixelType_Vec3f:
    case image::PixelType_Vec4f:
        return 4;

    case image::PixelType_Float64:
    case image::PixelType_Vec3d:
    case image::PixelType_Vec4d:
        return 8;
    }
    return 0;
}

namespace python_convert
{
    template<>
    CORE_API image::PixelType from_python(PyObject* obj)
    {
        if (PyUnicode_Check(obj))
            return (image::PixelType)image::string_to_pixel_type(PyUnicode_AsUTF8(obj));
        if (PyLong_Check(obj))
            return (image::PixelType)PyLong_AsLong(obj);
        else if (obj)
            PYTHON_ERROR_R(ValueError, image::PixelType_Unknown, "Failed to convert object of type '%s' to PixelType", obj->ob_type->tp_name);
        PYTHON_ERROR_R(ValueError, image::PixelType_Unknown, "NULL object");
    }

    template<>
    CORE_API PyObject* to_python(const image::PixelType& value)
    {
        return PyLong_FromLong((int)value);
    }
}
