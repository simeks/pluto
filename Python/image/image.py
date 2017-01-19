import numpy as np
import image.types

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
        self.pixel_type = getattr(obj, 'pixel_type', image.types.PixelType_Unknown)

    