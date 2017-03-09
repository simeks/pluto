import enum
import numpy as np
import re
from .types import *
from .image import Image

from flow import node_template, Pin

from matplotlib import cm

def colorize(img, min, max, cmap):
    colorfn = eval('cm.%s' % cmap)
    return (colorfn((img-min) / float(max-min)) * 255).astype(np.uint8)


node_template(
    title='PerceptuallyUniform',
    category='Image/Colormaps',
    pins={
        'In': Pin(Pin.In),
        'Min': Pin(Pin.In),
        'Max': Pin(Pin.In),
        'Out': Pin(Pin.Out)
    },
    properties={
        'colormap': EnumProperty('colormap', ['viridis', 'inferno', 'plasma', 'magma'], 0),
    },
    node_class = 'image.colormap.PerceptuallyUniformNode',
    func=perceptually_uniform
)

def perceptually_uniform(in, colormap, min=None, max=None):
    img = in
    if min == None:
        min = img.min()
    if max == None:
        max = img.max()

    out = Image(colorize(img, min, max, colormap), PixelType_Vec4u8)
    if hasattr(img, 'spacing'):
        out.spacing = img.spacing
    if hasattr(img, 'origin'):
        out.origin = img.origin
    return out


node_template(
    title='Jet',
    category='Image/Colormaps',
    pins={
        'In': Pin(Pin.In),
        'Min': Pin(Pin.In),
        'Max': Pin(Pin.In),
        'Out': Pin(Pin.Out)
    },
    node_class = 'image.colormap.JetNode',
    func=jet
)

def jet(in, min=None, max=None):
    img = in
    if min == None:
        min = img.min()
    if max == None:
        max = img.max()

    out = Image(colorize(img, min, max, 'jet'), PixelType_Vec4u8)
    if hasattr(img, 'spacing'):
        out.spacing = img.spacing
    if hasattr(img, 'origin'):
        out.origin = img.origin
    return out

