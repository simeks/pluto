import numpy as np
from flow import node
import image
from .types import *


@node('Magnitude', 'Image/Vectorfield')
def magnitude(img):
    """
    Args:
        In
    Returns:
        Out
    """
    if type(img) != image.Image:
        raise TypeError('Expected Image')

    n_comp = img.shape[-1]
    shape = img.shape[0:-1]
    img = img.reshape((np.product(shape), n_comp))
    m = np.zeros(np.product(shape))
    for i in range(0, n_comp):
        m = np.add(m, img[:,i]*img[:,i])
    
    m = np.sqrt(m)
    return image.Image(m.reshape(shape).astype('float32'), PixelType_Float32)