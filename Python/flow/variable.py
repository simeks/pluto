import flow
from pluto import pluto_class

@pluto_class
class VariableNode(flow.Node):
    pins = [
        flow.Pin('Out', flow.Pin.Out)
    ]
    properties = [
        flow.Property('name', ''),
        flow.Property('default', '')
    ]

    def __init__(self):
        super(VariableNode, self).__init__()
        self.node_class = 'flow.Variable'
        self.title = 'Variable'
        self.category = 'Flow'
        self.ui_class = 'variable_node'

    def run(self, ctx):
        if self.name in ctx.env:
            ctx.write_pin('Out', ctx.env[self.name])
        else:
            ctx.write_pin('Out', self.default)

flow.install_node_template(VariableNode())

