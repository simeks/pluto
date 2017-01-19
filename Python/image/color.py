import numpy as np
from flow import node
import image


@node('SplitChannels', 'Image')
def split_channels(img):
    """
    Args:
        Image
    Returns:
        R
        G
        B
        A
    """
    if type(img) != image.Image:
        raise TypeError('Expected Image')

    pixel_type = image.types.type_from_string(str(img.dtype))

    n_comp = image.types.num_components(img)
    if n_comp == 1:
        return img
    elif n_comp == 3:
        return (image.Image(img[::,::,0], pixel_type),
                image.Image(img[::,::,1], pixel_type), 
                image.Image(img[::,::,2], pixel_type))
    elif n_comp == 4:
        return (image.Image(img[::,::,0], pixel_type), 
                image.Image(img[::,::,1], pixel_type), 
                image.Image(img[::,::,2], pixel_type),
                image.Image(img[::,::,3], pixel_type))
