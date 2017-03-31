import numpy as np
import re
from .types import *
from .image import Image

from flow import node_template, Pin


def grid2d(width, height, step, thickness):
    h = height
    w = width
    img_data = np.zeros((h, w)).astype('uint8')
    
    for y in range(0, int(h / step)+1):
        for x in range(0, w):
            for t in range(0, min(thickness, h - y*step - 1)): 
                img_data[y*step+t,x] = 1
    for x in range(0, int(w / step)+1):
        for y in range(0, h):
            for t in range(0, min(thickness, w - x*step - 1)): 
                img_data[y,x*step+t] = 1
                
    return Image(img_data, PixelType_UInt8)

node_template(
    title='Grid2D',
    category='Image/New',
    pins={
        'Out': Pin(Pin.Out)
    },
    properties={
        'width': 0,
        'height': 0,
        'step': 1,
        'thickness': 1,
    },
    node_class = 'image.grid.Grid2DNode',
    func=grid2d
)
