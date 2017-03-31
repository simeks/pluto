import SimpleITK as sitk
from flow import FlowNode, FlowPin, FlowProperty, FileProperty, install_node_template
from pluto import pluto_class
import image
import os
import numpy as np

def format_from_sitk(img):
    n_comp = img.GetNumberOfComponentsPerPixel()
    pixel_id = img.GetPixelID()

    if pixel_id == sitk.sitkUInt8:
        return image.PixelType_UInt8
    elif pixel_id == sitk.sitkInt8:
        return image.PixelType_Int8
    elif pixel_id == sitk.sitkUInt16:
        return image.PixelType_UInt16
    elif pixel_id == sitk.sitkInt16:
        return image.PixelType_Int16
    elif pixel_id == sitk.sitkUInt32:
        return image.PixelType_UInt32
    elif pixel_id == sitk.sitkInt32:
        return image.PixelType_Int32
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
    elif fmt == image.PixelType_Int8:
        return sitk.sitkInt8, 1
    elif fmt == image.PixelType_UInt16:
        return sitk.sitkUInt16, 1
    elif fmt == image.PixelType_Int16:
        return sitk.sitkInt16, 1
    elif fmt == image.PixelType_UInt32:
        return sitk.sitkUInt32, 1
    elif fmt == image.PixelType_Int32:
        return sitk.sitkInt32, 1
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


def read(file):
    """
    Returns:
        Image : Loaded image
    """
    reader = sitk.ImageFileReader()
    reader.SetFileName(file)
    itk_img = reader.Execute()

    if itk_img == None:
        raise IOError('Failed to load image')

    fmt = format_from_sitk(itk_img)
    if fmt == None:
        raise IOError('Unknown format (SimpleITK PixelID: '+str(itk_img.GetPixelID())+')')

    img = image.Image(sitk.GetArrayFromImage(itk_img), fmt)
    img.spacing = itk_img.GetSpacing()
    img.origin = itk_img.GetOrigin()

    return img

def write(image, file):
    writer = sitk.ImageFileWriter()
    writer.SetFileName(str(file))

    if image is None:
        raise ValueError('Image object can not be None')

    _, n = format_to_sitk(image.pixel_type)

    out = sitk.GetImageFromArray(image, isVector=n!=1)

    out.SetSpacing(image.spacing)
    out.SetOrigin(image.origin)

    writer.Execute(out)


@pluto_class
class ReadNode(FlowNode):
    pins = [
        FlowPin('File', FlowPin.In),
        FlowPin('Image', FlowPin.Out)
    ]
    properties = [
        FileProperty('file', '', FileProperty.File_Open, "Image (*.*)"),
    ]

    def __init__(self):
        super(ReadNode, self).__init__()
        self.node_class = 'medkit.io.ReadNode'
        self.title = 'Read'
        self.category = 'Image/IO'

    def run(self, ctx):
        f = self.file
        if self.is_pin_linked('File'):
            f = ctx.read_pin('File')

        ctx.write_pin('Image', read(f))    


@pluto_class
class WriteNode(FlowNode):
    pins = [
        FlowPin('Image', FlowPin.In),
        FlowPin('File', FlowPin.In)
    ]
    properties = [
        FileProperty('file', '', FileProperty.File_Save, "Image (*.*)"),
    ]

    def __init__(self):
        super(WriteNode, self).__init__()
        self.node_class = 'medkit.io.WriteNode'
        self.title = 'Write'
        self.category = 'Image/IO'

    def run(self, ctx):
        f = self.file
        if self.is_pin_linked('File'):
            f = ctx.read_pin('File')

        im = ctx.read_pin('Image')
        if im is not None:
            if os.path.splitext(f)[1] == '.png':
                write(im.astype(np.uint8), f)
            else:
                write(im, f)

install_node_template(ReadNode())
install_node_template(WriteNode())
