import numpy as np
import re
from .types import *
from .image import Image

from flow import node_template, Pin

def ones(shape):
    if type(shape) == str:
        if re.match('\((\s*\d+\s*,?\s*)*\)$', shape.strip()) is None:
            raise ValueError('Invalid shape specifier: %s' % shape)
        shape = eval(shape)
    return Image(np.ones(shape))


def zeros(shape):
    if type(shape) == str:
        if re.match('\((\s*\d+\s*,?\s*)*\)$', shape.strip()) is None:
            raise ValueError('Invalid shape specifier: %s' % shape)
        shape = eval(shape)
    return Image(np.zeros(shape))

node_template(
    title='Ones',
    category='Image/New',
    ui={
        'ui_class': 'constant_node',
        'ui_node_title_var': 'shape'
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    properties={
        'shape': ''
    },
    node_class = 'image.new.OnesNode',
    func=ones
)

node_template(
    title='Zeros',
    category='Image/New',
    ui={
        'ui_class': 'constant_node',
        'ui_node_title_var': 'shape'
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    properties={
        'shape': ''
    },
    node_class = 'image.new.ZerosNode',
    func=zeros
)


