from flow import FlowNode, UiFlowNode, FlowPin, StringProperty, FileProperty, EnumProperty, install_node_template
from pluto import pluto_class

from .image import Image

@pluto_class
class ImageInfoNode(UiFlowNode):
    pins = [
        FlowPin('In', FlowPin.In)
    ]

    def __init__(self):
        super(ImageInfoNode, self).__init__()
        self.node_class = 'image.info.ImageInfoNode'
        self.title = 'Info'
        self.category = 'Image'
        self.ui_class = 'text_view_node'

    def run(self, ctx):
        img = ctx.read_pin('In')
        self.invoke_ui_method('set_text', '<b>Shape</b>: %s<br/><b>Format</b>: %s' % (str(img.shape), str(img.dtype)))

install_node_template(ImageInfoNode())

