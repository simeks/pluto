import numpy as np

from flow import install_node_template, FlowNode, FlowProperty, FlowPin, IntProperty

from .image import Image
from .types import *
from pluto import pluto_class
from scipy.ndimage import zoom 


@pluto_class
class ToWorldNode(FlowNode):
    pins = [
        FlowPin('In', FlowPin.In),
        FlowPin('Out', FlowPin.Out)
    ]

    properties = [
        IntProperty('order', 1)
    ]

    def __init__(self):
        super(ToWorldNode, self).__init__()
        self.node_class = 'image.resample.ToWorldNode'
        self.title = 'ToWorld'
        self.category = 'Image/Resample'
        self.ui_class = 'one_to_one_node'

    def run(self, ctx):
        img = ctx.read_pin('In')
        if hasattr(img, 'spacing'):
            out = Image(zoom(img, img.spacing[::-1], order=self.order), img.pixel_type)
            out.spacing = [1]*len(img.spacing)
            ctx.write_pin('Out', out)
        else:
            ctx.write_pin('Out', img)

install_node_template(ToWorldNode())
