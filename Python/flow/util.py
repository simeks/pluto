from flow import FlowNode, FlowPin, FlowProperty, install_node_template
from pluto import pluto_class

import numpy as np

@pluto_class
class EvalNode(FlowNode):
    pins = [
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        FlowProperty('code', ''),
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
        FlowProperty('file', '', FlowProperty.Type_FilePath),
    ]

    def __init__(self):
        super(FileBrowse, self).__init__()
        self.node_class = 'flow.util.FileBrowse'
        self.title = 'FileBrowse'
        self.category = 'Flow/Util'
        self.ui_class = 'single_pin_node'

    def run(self, ctx):
        ctx.write_pin('Out', self.file)


install_node_template(FileBrowse())
install_node_template(EvalNode())
