import image


def num_components(img):
    if type(img) == int:
        pixel_type = img
    else:
        pixel_type = img.pixel_type()

    if (pixel_type in [ image.PixelType_UInt8, 
                        image.PixelType_Int8, 
                        image.PixelType_UInt16, 
                        image.PixelType_Int16, 
                        image.PixelType_UInt32, 
                        image.PixelType_Int32, 
                        image.PixelType_Float32, 
                        image.PixelType_Float64]):
        return 1
    elif pixel_type in [image.PixelType_Vec3u8, image.PixelType_Vec3f, image.PixelType_Vec3d]:
        return 3
    elif pixel_type in [image.PixelType_Vec4u8, image.PixelType_Vec4f, image.PixelType_Vec4d]:
        return 4
    else:
        return 0