from flow import FlowNode, UiFlowNode, FlowPin, StringProperty, FileProperty, EnumProperty, install_node_template
from pluto import pluto_class

from .image import Image

import numpy as np


node_template(
    title='Info',
    category='Image/Info',
    ui={
        ui_class: 'text_view_node'
    },
    pins={
        'In': Pin(Pin.In)
    },
    node_class = 'image.info.ImageInfoNode',
    func=image_info
)

def image_info(ctx):
    img = ctx.read_pin('In')

    text = '<b>Shape</b>: %s<br/><b>Format</b>: %s' % (str(img.shape), str(img.dtype))
    if hasattr(img, 'spacing'):
        text += '<br/><b>Spacing</b>: %s' % str(img.spacing)
    if hasattr(img, 'origin'):
        text += '<br/><b>Origin</b>: %s' % str(img.origin)
    self.invoke_ui_method('set_text', text)


node_template(
    title='Shape',
    category='Image/Info',
    ui={
        ui_class: 'one_to_one_node'
    },
    pins={
        'Img': Pin(Pin.In),
        'Out': Pin(Pin.Out)
    },
    node_class = 'image.info.ShapeNode',
    func=image_shape
)

def image_shape(img):
    if img is None or not isinstance(img, np.ndarray):
        raise ValueError('Expected an Image object')
    return img.shape



node_template(
    title='DType',
    category='Image/Info',
    ui={
        ui_class: 'one_to_one_node'
    },
    pins={
        'Img': Pin(Pin.In),
        'Out': Pin(Pin.Out)
    },
    node_class = 'image.info.DTypeNode',
    func=image_dtype
)

def image_dtype(img):
    if img is None or not isinstance(img, np.ndarray):
        raise ValueError('Expected an Image object')
    return img.dtype

