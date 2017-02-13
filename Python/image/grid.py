import numpy as np
import re
from .types import *
from .image import Image

from flow import install_node_template, FlowNode, FlowProperty, FlowPin, IntProperty
from pluto import pluto_class


@pluto_class
class Grid2DNode(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        IntProperty('width', 0),
        IntProperty('height', 0),
        IntProperty('step', 1),
        IntProperty('thickness', 1),
    ]

    def __init__(self):
        super(Grid2DNode, self).__init__()
        self.node_class = 'image.grid.Grid2DNode'
        self.title = 'Grid2D'
        self.category = 'Image/New'
        self.ui_class = 'single_pin_node'

    def run(self, ctx):
        h = self.height
        w = self.width
        img_data = np.zeros((h, w)).astype('uint8')
        
        for y in range(0, int(h / self.step)+1):
            for x in range(0, w):
                for t in range(0, min(self.thickness, h - y*self.step - 1)): 
                    img_data[y*self.step+t,x] = 1
        for x in range(0, int(w / self.step)+1):
            for y in range(0, h):
                for t in range(0, min(self.thickness, w - x*self.step - 1)): 
                    img_data[y,x*self.step+t] = 1
                    
        ctx.write_pin('Out', Image(img_data, PixelType_UInt8))

install_node_template(Grid2DNode())

