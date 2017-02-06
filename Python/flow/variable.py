from flow import FlowNode, FlowPin, StringProperty, install_node_template
from pluto import pluto_class

@pluto_class
class VariableNode(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        StringProperty('name', ''),
        StringProperty('default', '')
    ]

    def __init__(self):
        super(VariableNode, self).__init__()
        self.node_class = 'flow.variable.VariableNode'
        self.title = 'Variable'
        self.category = 'Flow'
        self.ui_class = 'variable_node'

    def run(self, ctx):
        if self.name in ctx.env:
            ctx.write_pin('Out', ctx.env[self.name])
        else:
            ctx.write_pin('Out', self.default)

install_node_template(VariableNode())

