import flow
from pluto import pluto_class

@pluto_class
class TerminalNode(flow.Node):
    pins = [
        flow.Pin('Out', flow.Pin.Out)
    ]
    properties = [
        flow.Property('name', ''),
        flow.Property('value', '')
    ]

    def __init__(self):
        super(TerminalNode, self).__init__()
        self.node_class = 'flow.Terminal'
        self.title = 'Terminal'
        self.category = 'Flow'

    def run(self, ctx):
        pass

flow.install_node_template(TerminalNode())