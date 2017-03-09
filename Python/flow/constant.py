from flow import node_template, Pin


node_template(
    title='Bool',
    category='Constants',
    properties={
        'value': False
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    doc='Boolean constant',
    func=BoolNode
)

def BoolNode(value):
    return value


node_template(
    title='Integer',
    category='Constants',
    properties={
        'value': 0
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    doc='Integer constant',
    func=IntNode
)

def IntNode(value):
    return value


node_template(
    title='Float',
    category='Constants',
    properties={
        'value': 0.0
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    doc='Float constant',
    func=FloatNode
)

def FloatNode(value):
    return value


node_template(
    title='String',
    category='Constants',
    properties={
        'value': ''
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    doc='String constant',
    func=StringNode
)

def StringNode(value):
    return value
