from flow import FlowNode, ArrayFlowPin, FlowPin, StringProperty, FileProperty, EnumProperty, install_node_template
from pluto import pluto_class
import re

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

@pluto_class
class IndexNode(FlowNode):
    pins = [
        FlowPin('In', FlowPin.In),
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        StringProperty('index', '0'),
    ]

    def __init__(self):
        super(IndexNode, self).__init__()
        self.node_class = 'flow.list.IndexNode'
        self.title = 'Slice'
        self.category = 'List'
        self.ui_class = 'one_to_one_node'
        self.ui_node_title_var = 'index'

    def run(self, ctx):
        lst = ctx.read_pin('In')

        # Validate index
        index = self.index.strip()
        if not re.match('[\-0-9:]+$', index):
            raise ValueError('Invalid index')

        ctx.write_pin('Out', eval('lst[%s]' % index))

install_node_template(BuildNode())
install_node_template(ConcatenateNode())
install_node_template(IndexNode())

