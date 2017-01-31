import flow
from pluto import pluto_class

@pluto_class
class ConstantNode(flow.Node):
    pins = [
        flow.Pin('Out', flow.Pin.Out)
    ]
    properties = [
        flow.Property('value', ''),
        flow.Property('type', 'str'),
    ]

    def __init__(self):
        super(ConstantNode, self).__init__()
        self.node_class = 'flow.Constant'
        self.title = 'Constant'
        self.category = 'Flow'
        self.ui_class = 'constant_node'

    def run(self, ctx):
        if self.type.lower() == 'str':
            v = self.value
        elif self.type.lower() == 'int':
            v = int(self.value)
        elif self.type.lower() == 'float':
            v = float(self.value)
        else:
            raise ValueError('Unsupported type: \'%s\'' % self.type)

        ctx.write_pin('Out', v)


flow.install_node_template(ConstantNode())

