import numpy as np
import re
from image.types import *

from flow import install_node_template, FlowNode, FlowProperty, FlowPin, StringProperty
from pluto import pluto_class


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


@pluto_class
class SliceImageNode(FlowNode):
    pins = [
        FlowPin('In', FlowPin.In),
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        StringProperty('index', '0,0'),
    ]

    def __init__(self):
        super(SliceImageNode, self).__init__()
        self.node_class = 'image.image.SliceImageNode'
        self.title = 'Slice'
        self.category = 'Image/Slice'
        self.ui_class = 'one_to_one_node'
        self.ui_node_title_var = 'index'

    def run(self, ctx):
        img = ctx.read_pin('In')
        if img is None or not isinstance(img, np.ndarray):
            raise ValueError('Expected an Image object')

        # Validate index
        index = ''
        tokens = self.index.split(',')
        for i in range(0, len(tokens)):
            if i != len(tokens)-1 and tokens[i] == '': # Allow a trailing ','
                index = index + ','
                continue
            if re.match('[0-9:]+$', tokens[i].strip()):
                index = index + tokens[i].strip()
                if i != len(tokens)-1:
                    index = index + ','
            else:
                raise SyntaxError('Invalid syntax: %s' % self.index) 

        ctx.write_pin('Out', eval('img[%s]' % index))



@pluto_class
class SetSliceImageNode(FlowNode):
    pins = [
        FlowPin('In', FlowPin.In),
        FlowPin('Value', FlowPin.In),
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        StringProperty('slice', '0,0'),
    ]

    def __init__(self):
        super(SetSliceImageNode, self).__init__()
        self.node_class = 'image.image.SetSliceImageNode'
        self.title = 'SetSlice'
        self.category = 'Image/Slice'

    def run(self, ctx):
        img = ctx.read_pin('In')
        if img is None or not isinstance(img, np.ndarray):
            raise ValueError('Expected an Image object')

        value = ctx.read_pin('Value')

        # Validate index
        index = ''
        tokens = self.slice.split(',')
        for i in range(0, len(tokens)):
            if i != len(tokens)-1 and tokens[i] == '': # Allow a trailing ','
                index = index + ','
                continue
            if re.match('[0-9:]+$', tokens[i].strip()):
                index = index + tokens[i].strip()
                if i != len(tokens)-1:
                    index = index + ','
            else:
                raise SyntaxError('Invalid syntax: %s' % self.slice) 

        tmp = img
        exec('tmp[%s] = value' % index)
        ctx.write_pin('Out', tmp)

install_node_template(SliceImageNode())
install_node_template(SetSliceImageNode())

