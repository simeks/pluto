import flow
from pluto import pluto_class

@pluto_class
class ResultNode(flow.Node):
    pins = [
        flow.Pin('In', flow.Pin.In)
    ]

    def __init__(self):
        super(ResultNode, self).__init__()
        self.node_class = 'flow.Result'
        self.title = 'Result'
        self.category = ''
        self.value = ''

    def run(self, ctx):
        self.value = str(ctx.read_pin('In'))


flow.install_node_template(ResultNode())

