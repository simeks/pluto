import numpy as np
from image.types import *

class Image(np.ndarray):
    def __new__(cls, arr, pixel_type):
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




    