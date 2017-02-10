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
        self.category = 'Image/Info'
        self.ui_class = 'text_view_node'

    def run(self, ctx):
        img = ctx.read_pin('In')
        self.invoke_ui_method('set_text', '<b>Shape</b>: %s<br/><b>Format</b>: %s' % (str(img.shape), str(img.dtype)))

@pluto_class
class ShapeNode(FlowNode):
    pins = [
        FlowPin('In', FlowPin.In),
        FlowPin('Out', FlowPin.Out)
    ]

    def __init__(self):
        super(ShapeNode, self).__init__()
        self.node_class = 'image.info.ShapeNode'
        self.title = 'Shape'
        self.category = 'Image/Info'
        self.ui_class = 'one_to_one_node'

    def run(self, ctx):
        img = ctx.read_pin('In')
        if img is None or not isinstance(img, np.ndarray):
            raise ValueError('Expected an Image object')

        ctx.write_pin('Out', img.shape)

@pluto_class
class DTypeNode(FlowNode):
    pins = [
        FlowPin('In', FlowPin.In),
        FlowPin('Out', FlowPin.Out)
    ]

    def __init__(self):
        super(DTypeNode, self).__init__()
        self.node_class = 'image.info.DTypeNode'
        self.title = 'DType'
        self.category = 'Image/Info'
        self.ui_class = 'one_to_one_node'

    def run(self, ctx):
        img = ctx.read_pin('In')
        if img is None or not isinstance(img, np.ndarray):
            raise ValueError('Expected an Image object')

        ctx.write_pin('Out', str(img.dtype))


install_node_template(ImageInfoNode())
install_node_template(ShapeNode())
install_node_template(DTypeNode())
