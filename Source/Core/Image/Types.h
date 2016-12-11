#ifndef __CORE_IMAGE_TYPES_H__
#define __CORE_IMAGE_TYPES_H__

#include <Core/Common.h>
#include <Core/Image/Vec3.h>
#include <Core/Image/Color.h>

namespace image
{
    enum PixelType
    {
        PixelType_Unknown,
        PixelType_UInt8,
        PixelType_UInt16,
        PixelType_UInt32,
        PixelType_Float32,
        PixelType_Float64,
        PixelType_Vec3u8,
        PixelType_Vec3f,
        PixelType_Vec3d,
        PixelType_Vec4u8,
        PixelType_Vec4f,
        PixelType_Vec4d
    };
    
    size_t pixel_size(int type);
    int string_to_pixel_type(const char* str);
    const char* pixel_type_to_string(int type);

    /// Number of components for the given type
    int num_components(int type);
    /// Component size in bytes
    size_t component_size(int type);

    template<typename T>
    struct pixel_type
    {
        typedef T ValueType;
        enum {
            TypeId = image::PixelType_Unknown
        };
    };


    #define PIXEL_TYPE_TRAIT(Type, Id) \
        template<> \
        struct pixel_type<Type> \
        { \
            typedef Type ValueType; \
            enum { \
                TypeId = Id \
            }; \
        }; 

    PIXEL_TYPE_TRAIT(uint8_t, image::PixelType_UInt8);
    PIXEL_TYPE_TRAIT(uint16_t, image::PixelType_UInt16);
    PIXEL_TYPE_TRAIT(uint32_t, image::PixelType_UInt32);
    PIXEL_TYPE_TRAIT(float, image::PixelType_Float32);
    PIXEL_TYPE_TRAIT(double, image::PixelType_Float64);
    PIXEL_TYPE_TRAIT(Vec3u8, image::PixelType_Vec3u8);
    PIXEL_TYPE_TRAIT(Vec3f, image::PixelType_Vec3f);
    PIXEL_TYPE_TRAIT(Vec3d, image::PixelType_Vec3d);
    PIXEL_TYPE_TRAIT(RGBA32, image::PixelType_Vec4u8);
    PIXEL_TYPE_TRAIT(Colorf, image::PixelType_Vec4f);
    PIXEL_TYPE_TRAIT(Colord, image::PixelType_Vec4d);

}



#endif // __CORE_IMAGE_TRAITS_H__
