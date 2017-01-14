import numpy as np
import imageutils
from flow import node
from image import Image

@node('SplitChannels', 'Image')
def split_channels(image):
    """
    Args:
        Image
    Returns:
        R
        G
        B
        A
    """
    if type(image) != Image:
        raise TypeError('Expected Image')

    arr = image.array()
    dtype = str(arr.dtype)

    n_comp = imageutils.num_components(image)
    if n_comp == 1:
        return image
    elif n_comp == 3:
        return (Image(arr[::,::,0], dtype), 
                Image(arr[::,::,1], dtype), 
                Image(arr[::,::,2], dtype))
    elif n_comp == 4:
        return (Image(arr[::,::,0], dtype), 
                Image(arr[::,::,1], dtype), 
                Image(arr[::,::,2], dtype),
                Image(arr[::,::,3], dtype))
