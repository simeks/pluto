import flow
from pluto import pluto_class

@pluto_class
class PrintNode(flow.Node):
    pins = [
        flow.Pin('In', flow.Pin.In)
    ]

    def __init__(self):
        super(PrintNode, self).__init__()
        self.node_class = 'flow.Print'
        self.title = 'Print'
        self.category = 'Flow'
        self.value = ''
        self.ui_class = 'print_node'

    def run(self, ctx):
        self.value = str(ctx.read_pin('In'))
        print(self.value)


flow.install_node_template(PrintNode())

