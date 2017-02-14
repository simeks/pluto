import numpy as np
import re
from .types import *
from .image import Image

from flow import install_node_template, FlowNode, FlowProperty, FlowPin, IntProperty, EnumProperty
from pluto import pluto_class

from matplotlib import cm

def colorize(img, min, max, cmap):
    colorfn = eval('cm.%s' % cmap)
    return colorfn((img-min) / float(max-min)) * 255


@pluto_class
class PerceptuallyUniformNode(FlowNode):
    pins = [
        FlowPin('In', FlowPin.In),
        FlowPin('Min', FlowPin.In),
        FlowPin('Max', FlowPin.In),
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        EnumProperty('colormap', ['viridis', 'inferno', 'plasma', 'magma'], 0),
    ]

    def __init__(self):
        super(PerceptuallyUniformNode, self).__init__()
        self.node_class = 'image.colormap.PerceptuallyUniformNode'
        self.title = 'PerceptuallyUniform'
        self.category = 'Image/Colormaps'

    def run(self, ctx):
        img = ctx.read_pin('In')

        if self.is_pin_linked('Min'):
            min = ctx.read_pin('Min')
        else:
            min = img.min()
        if self.is_pin_linked('Max'):
            max = ctx.read_pin('Max')
        else:
            max = img.max()

        ctx.write_pin('Out', Image(colorize(img, min, max, self.colormap), PixelType_Vec4u8))

@pluto_class
class JetNode(FlowNode):
    pins = [
        FlowPin('In', FlowPin.In),
        FlowPin('Min', FlowPin.In),
        FlowPin('Max', FlowPin.In),
        FlowPin('Out', FlowPin.Out)
    ]

    def __init__(self):
        super(JetNode, self).__init__()
        self.node_class = 'image.colormap.JetNode'
        self.title = 'Jet'
        self.category = 'Image/Colormaps'

    def run(self, ctx):
        img = ctx.read_pin('In')

        if self.is_pin_linked('Min'):
            min = ctx.read_pin('Min')
        else:
            min = img.min()
        if self.is_pin_linked('Max'):
            max = ctx.read_pin('Max')
        else:
            max = img.max()

        ctx.write_pin('Out', Image(colorize(img, min, max, 'jet'), PixelType_Vec4u8))


install_node_template(PerceptuallyUniformNode())
install_node_template(JetNode())

