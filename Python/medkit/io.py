import SimpleITK as sitk
from flow import node
import image

def format_from_sitk(img):
    n_comp = img.GetNumberOfComponentsPerPixel()
    pixel_id = img.GetPixelID()

    if pixel_id == sitk.sitkUInt8:
        return image.PixelType_UInt8
    elif pixel_id == sitk.sitkUInt16:
        return image.PixelType_UInt16
    elif pixel_id == sitk.sitkUInt32:
        return image.PixelType_UInt32
    elif pixel_id == sitk.sitkFloat32:
        return image.PixelType_Float32
    elif pixel_id == sitk.sitkFloat64:
        return image.PixelType_Float64
    elif pixel_id == sitk.sitkVectorUInt8:
        if n_comp == 3:
            return image.PixelType_Vec3u8
        elif n_comp == 4:
            return image.PixelType_Vec4u8
    elif pixel_id == sitk.sitkVectorFloat32:
        if n_comp == 3:
            return image.PixelType_Vec3f
        elif n_comp == 4:
            return image.PixelType_Vec4f
    elif pixel_id == sitk.sitkVectorFloat64:
        if n_comp == 3:
            return image.PixelType_Vec3d
        elif n_comp == 4:
            return image.PixelType_Vec4d


def format_to_sitk(fmt):
    if fmt == image.PixelType_UInt8:
        return sitk.sitkUInt8, 1
    elif fmt == image.PixelType_UInt16:
        return sitk.sitkUInt16, 1
    elif fmt == image.PixelType_UInt32:
        return sitk.sitkUInt32, 1
    elif fmt == image.PixelType_Float32:
        return sitk.sitkFloat32, 1
    elif fmt == image.PixelType_Float64:
        return sitk.sitkFloat64, 1
    elif fmt == image.PixelType_Vec3u8:
        return sitk.sitkVectorUInt8, 3
    elif fmt == image.PixelType_Vec4u8:
        return sitk.sitkVectorUInt8, 4
    elif fmt == image.PixelType_Vec3f:
        return sitk.sitkVectorFloat32, 3
    elif fmt == image.PixelType_Vec4f:
        return sitk.sitkVectorFloat32, 4
    elif fmt == image.PixelType_Vec3d:
        return sitk.sitkVectorFloat64, 3
    elif fmt == image.PixelType_Vec4d:
        return sitk.sitkVectorFloat64, 4

@node('Read', 'MedKit')
def read(File):
    """
    Returns:
    Image : Loaded image
    """
    reader = sitk.ImageFileReader()
    reader.SetFileName(File)
    itk_img = reader.Execute()

    fmt = format_from_sitk(itk_img)
    if fmt == None:
        raise IOError('Unknown format (SimpleITK PixelID: '+str(itk_img.GetPixelID())+')')

    img = image.Image(sitk.GetArrayFromImage(itk_img), fmt)
    img.set_spacing(itk_img.GetSpacing())
    img.set_origin(itk_img.GetOrigin())

    return img

@node('Write', 'MedKit')
def write(Image, File):
    writer = sitk.ImageFileWriter()
    writer.SetFileName(str(File))

    _, n = format_to_sitk(Image.pixel_type())

    out = sitk.GetImageFromArray(Image.array(), isVector=n!=1)

    out.SetSpacing(Image.spacing())
    out.SetOrigin(Image.origin())

    writer.Execute(out)