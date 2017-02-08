from flow import FlowNode, FlowPin, StringProperty, FileProperty, EnumProperty, install_node_template
from pluto import pluto_class

import numpy as np

@pluto_class
class EvalNode(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        StringProperty('code', ''),
    ]

    def __init__(self):
        super(EvalNode, self).__init__()
        self.node_class = 'flow.util.Eval'
        self.title = 'Eval'
        self.category = 'Flow'
        self.ui_class = 'single_pin_node'

    def run(self, ctx):
        ctx.write_pin('Out', eval(self.code))



@pluto_class
class FileBrowse(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        FileProperty('file', ''),
    ]

    def __init__(self):
        super(FileBrowse, self).__init__()
        self.node_class = 'flow.util.FileBrowse'
        self.title = 'FileBrowse'
        self.category = 'Flow/Util'
        self.ui_class = 'single_pin_node'

    def run(self, ctx):
        ctx.write_pin('Out', self.file)



@pluto_class
class BuildList(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out),
        FlowPin('0', FlowPin.In),
        FlowPin('1', FlowPin.In),
        FlowPin('2', FlowPin.In),
        FlowPin('3', FlowPin.In),
        FlowPin('4', FlowPin.In),
        FlowPin('5', FlowPin.In),
        FlowPin('6', FlowPin.In),
        FlowPin('7', FlowPin.In),
        FlowPin('8', FlowPin.In),
        FlowPin('9', FlowPin.In)
    ]

    def __init__(self):
        super(BuildList, self).__init__()
        self.node_class = 'flow.util.BuildList'
        self.title = 'BuildList'
        self.category = 'Flow/Util'

    def run(self, ctx):
        lst = []
        for i in range(0, 10):
            if self.is_pin_linked(str(i)):
                lst.append(ctx.read_pin(str(i)))
        ctx.write_pin('Out', lst)


install_node_template(FileBrowse())
install_node_template(EvalNode())
install_node_template(BuildList())
