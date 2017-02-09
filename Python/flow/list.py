from flow import FlowNode, ArrayFlowPin, FlowPin, StringProperty, FileProperty, EnumProperty, install_node_template
from pluto import pluto_class


@pluto_class
class BuildNode(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out),
        ArrayFlowPin('Item', FlowPin.In),
    ]

    def __init__(self):
        super(BuildNode, self).__init__()
        self.node_class = 'flow.list.BuildNode'
        self.title = 'Build'
        self.category = 'List'

    def run(self, ctx):
        lst = ctx.read_pin('Item')
        ctx.write_pin('Out', lst)

@pluto_class
class ConcatenateNode(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out),
        ArrayFlowPin('Item', FlowPin.In),
    ]

    def __init__(self):
        super(ConcatenateNode, self).__init__()
        self.node_class = 'flow.list.ConcatenateNode'
        self.title = 'Concatenate'
        self.category = 'List'

    def run(self, ctx):
        lst = ctx.read_pin('Item')
        out = []
        for l in lst:
            out.extend(l)
        ctx.write_pin('Out', out)


install_node_template(BuildNode())
install_node_template(ConcatenateNode())

