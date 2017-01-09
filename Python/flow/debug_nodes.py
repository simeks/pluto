import flow
from flow.node_helper import context_node, node
from pluto import pluto_class
import time

@pluto_class
class SleepNode(flow.Node):
    pins = [
        flow.Pin('In', flow.Pin.In),
        flow.Pin('Out', flow.Pin.Out)
    ]
    properties = [
        flow.Property('time', '0'),
    ]

    def __init__(self):
        super(SleepNode, self).__init__()
        self.node_class = 'flow.debug.Sleep'
        self.title = 'Sleep'
        self.category = 'Debug'
        self.value = ''

    def run(self, ctx):
    	time.sleep(int(self.time))
    	ctx.write_pin('Out', ctx.read_pin('In'))


flow.install_node_template(SleepNode())


@pluto_class
class FailNode(flow.Node):
    pins = [
        flow.Pin('In', flow.Pin.In),
    ]
    properties = [
        flow.Property('error', ''),
    ]

    def __init__(self):
        super(FailNode, self).__init__()
        self.node_class = 'flow.debug.Fail'
        self.title = 'Fail'
        self.category = 'Debug'
        self.value = ''

    def run(self, ctx):
        if self.error != '':
            raise ValueError(self.error)


flow.install_node_template(FailNode())

