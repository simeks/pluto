import flow
from pluto import pluto_class

@pluto_class
class ConstantNode(flow.Node):
    pins = [
        flow.Pin('Out', flow.Pin.Out)
    ]
    properties = [
        flow.Property('value', '')
    ]

    def __init__(self):
        super(ConstantNode, self).__init__()
        self.node_class = 'flow.Constant'
        self.title = 'Constant'
        self.category = ''

    def run(self, ctx):
        ctx.write_pin('Out', self.value)


flow.install_node_template(ConstantNode())

