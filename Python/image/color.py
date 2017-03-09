import numpy as np
from flow import node_template, Pin
import image


node_template(
    title='SplitChannels',
    category='Image',
    pins={
        'Imgage': Pin(Pin.In),
        'R': Pin(Pin.Out),
        'G': Pin(Pin.Out),
        'B': Pin(Pin.Out),
        'A': Pin(Pin.Out)
    },
    func=split_channels
)

def split_channels(image):
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
