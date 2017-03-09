import numpy as np

from flow import node_template, Pin

from .image import Image
from .types import *
from scipy.ndimage import zoom 

node_template(
    title='ToWorld',
    category='Image/Resample',
    ui={
        ui_class: 'one_to_one_node',
    },
    pins={
        'Img': Pin(Pin.In),
        'Out': Pin(Pin.Out)
    },
    properties={
        'order': 1
    },
    node_class='image.resample.ToWorldNode',
    func=to_world
)

def to_world(img, order):
    if hasattr(img, 'spacing'):
        out = Image(zoom(img, img.spacing[::-1], order=order), img.pixel_type)
        out.spacing = [1]*len(img.spacing)
        return out
    else:
        return out

