import numpy as np
import re
from .types import *
from .image import Image

from flow import install_node_template, FlowNode, FlowProperty, FlowPin, StringProperty
from pluto import pluto_class


@pluto_class
class OnesNode(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        StringProperty('shape', '(0, 0)'),
    ]

    def __init__(self):
        super(OnesNode, self).__init__()
        self.node_class = 'image.new.OnesNode'
        self.title = 'Ones'
        self.category = 'Image/New'
        self.ui_class = 'constant_node'
        self.ui_node_title_var = 'shape'

    def run(self, ctx):
        if re.match('\((\s*\d+\s*,?\s*)*\)$', self.shape.strip()) is None:
            raise ValueError('Invalid shape specifier: %s' % self.shape)

        shape = eval(self.shape)
        ctx.write_pin('Out', Image(np.ones(shape)))


@pluto_class
class ZerosNode(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        StringProperty('shape', '(0, 0)'),
    ]

    def __init__(self):
        super(ZerosNode, self).__init__()
        self.node_class = 'image.new.ZerosNode'
        self.title = 'Zeros'
        self.category = 'Image/New'
        self.ui_class = 'constant_node'
        self.ui_node_title_var = 'shape'

    def run(self, ctx):
        if re.match('\((\s*\d+\s*,?\s*)*\)$', self.shape.strip()) is None:
            raise ValueError('Invalid shape specifier: %s' % self.shape)

        shape = eval(self.shape)
        ctx.write_pin('Out', Image(np.zeros(shape)))

install_node_template(OnesNode())
install_node_template(ZerosNode())

