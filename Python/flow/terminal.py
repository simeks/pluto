import flow

class TerminalNode(flow.Node):
    def __init__(self):
        super(TerminalNode, self).__init__()
        self.node_class = 'flow.Terminal'
        self.title = 'Terminal'
        self.category = 'Flow'

        self.add_pin('Out', flow.Pin.Out)

    def run(self, ctx):
        pass

flow.install_node_template(TerminalNode())