import numpy as np
import re
from image.types import *

from flow import node_template, Pin


class Image(np.ndarray):
    def __new__(cls, arr, pixel_type=PixelType_Unknown):
        obj = np.asarray(arr).view(cls)
        obj.spacing = (1, 1, 1)
        obj.origin = (0, 0, 0)
        obj.pixel_type = pixel_type
        return obj

    def __array_finalize__(self, obj):
        if obj is None:
            return

        self.spacing = getattr(obj, 'spacing', (1, 1, 1))
        self.origin = getattr(obj, 'origin', (0, 0, 0))
        self.pixel_type = getattr(obj, 'pixel_type', PixelType_Unknown)

        if len(obj.shape) == 0:
            return # TODO: Don't know what is happening here

        # Lets say we have I = ImageVec3d, if we do I[:,:,1]
        if self.shape == obj.shape[:-1]:
            if obj.shape[-1] == 3:
                if self.pixel_type == PixelType_Vec3u8:
                    self.pixel_type = PixelType_UInt8
                elif self.pixel_type == PixelType_Vec3f:
                    self.pixel_type = PixelType_Float32
                elif self.pixel_type == PixelType_Vec3d:
                    self.pixel_type = PixelType_Float64

            elif obj.shape[-1] == 4:
                if self.pixel_type == PixelType_Vec4u8:
                    self.pixel_type = PixelType_UInt8
                elif self.pixel_type == PixelType_Vec4f:
                    self.pixel_type = PixelType_Float32
                elif self.pixel_type == PixelType_Vec4d:
                    self.pixel_type = PixelType_Float64

        if self.dtype != obj.dtype:
            if self.pixel_type in (PixelType_Vec3u8, PixelType_Vec3f, PixelType_Vec3d):
                if self.dtype == 'uint8':
                    self.pixel_type = PixelType_Vec3u8
                elif self.dtype == 'float32':
                    self.pixel_type = PixelType_Vec3f
                elif self.dtype == 'float64':
                    self.pixel_type = PixelType_Vec3d
                else:
                    raise ValueError('Failed to convert')
            if self.pixel_type in (PixelType_Vec4u8, PixelType_Vec4f, PixelType_Vec4d):
                if self.dtype == 'uint8':
                    self.pixel_type = PixelType_Vec4u8
                elif self.dtype == 'float32':
                    self.pixel_type = PixelType_Vec4f
                elif self.dtype == 'float64':
                    self.pixel_type = PixelType_Vec4d
                else:
                    raise ValueError('Failed to convert')
            else:
                self.pixel_type = type_from_string(str(self.dtype))


node_template(
    title='Spacing',
    category='Image/Meta',
    ui={
        ui_class = 'one_to_one_node'
    },
    pins={
        'Img': Pin(Pin.In),
        'Out': Pin(Pin.Out)
    },
    node_class = 'image.image.ImageSpacingNode',
    func=perceptually_uniform
)

def spacing(img):
    return img.spacing


node_template(
    title='Slice',
    category='Image/Slice',
    ui={
        ui_class: 'one_to_one_node',
        ui_node_title_var: 'index'
    },
    pins={
        'Img': Pin(Pin.In),
        'Out': Pin(Pin.Out)
    },
    properties={
        'index': '0,0'
    },
    node_class = 'image.image.SliceImageNode',
    func=image_slice
)

def image_slice(img, index):
    if img is None or not isinstance(img, np.ndarray):
        raise ValueError('Expected an Image object')

    # Validate index
    vindex = ''
    tokens = index.split(',')
    for i in range(0, len(tokens)):
        if i != len(tokens)-1 and tokens[i] == '': # Allow a trailing ','
            vindex = vindex + ','
            continue
        if re.match('[\-0-9:]+$', tokens[i].strip()):
            vindex = vindex + tokens[i].strip()
            if i != len(tokens)-1:
                vindex = vindex + ','
        else:
            raise SyntaxError('Invalid syntax: %s' % index) 

    return eval('img[%s]' % vindex)


node_template(
    title='SetSlice',
    category='Image/Slice',
    ui={
        ui_class: 'one_to_one_node',
        ui_node_title_var: 'index'
    },
    pins={
        'Img': Pin(Pin.In),
        'Value': Pin(Pin.In),
        'Out': Pin(Pin.Out)
    },
    properties={
        'index': '0,0'
    },
    node_class = 'image.image.SetSliceImageNode',
    func=image_set_slice
)

def image_set_slice(img, value, index):
    img = ctx.read_pin('In')
    if img is None or not isinstance(img, np.ndarray):
        raise ValueError('Expected an Image object')

    # Validate index
    vindex = ''
    tokens = index.split(',')
    for i in range(0, len(tokens)):
        if i != len(tokens)-1 and tokens[i] == '': # Allow a trailing ','
            vindex = vindex + ','
            continue
        if re.match('[\-0-9:]+$', tokens[i].strip()):
            vindex = vindex + tokens[i].strip()
            if i != len(tokens)-1:
                vindex = vindex + ','
        else:
            raise SyntaxError('Invalid syntax: %s' % index)

    tmp = img
    exec('tmp[%s] = value' % index)
    return tmp
