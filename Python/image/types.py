from image_api import *


def num_components(img):
    if type(img) == int:
        pixel_type = img
    else:
        pixel_type = img.pixel_type

    if (pixel_type in [ PixelType_UInt8, 
                        PixelType_Int8, 
                        PixelType_UInt16, 
                        PixelType_Int16, 
                        PixelType_UInt32, 
                        PixelType_Int32, 
                        PixelType_Float32, 
                        PixelType_Float64]):
        return 1
    elif pixel_type in [PixelType_Vec3u8, PixelType_Vec3f, PixelType_Vec3d]:
        return 3
    elif pixel_type in [PixelType_Vec4u8, PixelType_Vec4f, PixelType_Vec4d]:
        return 4
    else:
        return 0
