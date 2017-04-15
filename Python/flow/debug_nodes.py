from flow import Pin, node_template
from time import sleep

def Sleep(In, time):
    sleep(int(time))
    return In

def Fail(In, error):
    raise ValueError(error)

node_template(
    title='Sleep',
    category='Debug',
    properties={
        'time': 0
    },
    pins={
        'In': Pin(Pin.In),
        'Out': Pin(Pin.Out)
    },
    doc='Sleep',
    node_class='flow.debug.Sleep',
    func=Sleep
)


node_template(
    title='Fail',
    category='Debug',
    properties={
        'error': ''
    },
    pins={
        'In': Pin(Pin.In),
    },
    doc='Raises an error with the specified error message',
    node_class='flow.debug.Fail',
    func=Fail
)

