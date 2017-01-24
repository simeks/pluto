import flow
from pluto import pluto_class

import numpy as np

@pluto_class
class EvalNode(flow.Node):
    pins = [
        flow.Pin('Out', flow.Pin.Out)
    ]
    properties = [
        flow.Property('code', ''),
    ]

    def __init__(self):
        super(EvalNode, self).__init__()
        self.node_class = 'flow.util.Eval'
        self.title = 'Eval'
        self.category = 'Flow'

    def run(self, ctx):
        ctx.write_pin('Out', eval(self.code))


flow.install_node_template(EvalNode())
