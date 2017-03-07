from flow import FlowNode, FlowPin, StringProperty, IntProperty, install_node_template
from flow.node_helper import context_node, node
from pluto import pluto_class
import time


node_template(
    title='Bool',
    category='Constants',
    properties={
        'time': IntProperty(0)
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    doc='Boolean constant',
    node_class='flow.debug.Sleep',
    func=Sleep
)
time.sleep(int(self.time))
ctx.write_pin('Out', ctx.read_pin('In'))


@pluto_class
class SleepNode(FlowNode):
    pins = [
        FlowPin('In', FlowPin.In),
        FlowPin('Out', FlowPin.Out)
    ]
    properties = [
        IntProperty('time', 0),
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


install_node_template(SleepNode())


@pluto_class
class FailNode(FlowNode):
    pins = [
        FlowPin('In', FlowPin.In),
    ]
    properties = [
        StringProperty('error', ''),
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


install_node_template(FailNode())

