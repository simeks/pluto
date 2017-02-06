from flow import install_node_template, FlowNode, FlowProperty, FlowPin, BoolProperty, IntProperty, FloatProperty, StringProperty
from pluto import pluto_class


@pluto_class
class BoolNode(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        BoolProperty('value', False),
    ]

    def __init__(self):
        super(BoolNode, self).__init__()
        self.node_class = 'flow.constant.BoolNode'
        self.title = 'Bool'
        self.category = 'Constants'
        self.ui_class = 'constant_node'

    def run(self, ctx):
        ctx.write_pin('Out', self.value)


@pluto_class
class IntNode(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        IntProperty('value', 0),
    ]

    def __init__(self):
        super(IntNode, self).__init__()
        self.node_class = 'flow.constant.IntNode'
        self.title = 'Integer'
        self.category = 'Constants'
        self.ui_class = 'constant_node'

    def run(self, ctx):
        ctx.write_pin('Out', self.value)


@pluto_class
class FloatNode(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        FloatProperty('value', 0),
    ]

    def __init__(self):
        super(FloatNode, self).__init__()
        self.node_class = 'flow.constant.FloatNode'
        self.title = 'Float'
        self.category = 'Constants'
        self.ui_class = 'constant_node'

    def run(self, ctx):
        ctx.write_pin('Out', self.value)


@pluto_class
class StringNode(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        StringProperty('value', ''),
    ]

    def __init__(self):
        super(StringNode, self).__init__()
        self.node_class = 'flow.constant.StringNode'
        self.title = 'String'
        self.category = 'Constants'
        self.ui_class = 'constant_node'

    def run(self, ctx):
        ctx.write_pin('Out', self.value)


install_node_template(BoolNode())
install_node_template(IntNode())
install_node_template(FloatNode())
install_node_template(StringNode())