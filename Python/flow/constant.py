from flow import node_template, Pin, BoolProperty, IntProperty, FloatProperty, StringProperty


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

def BoolNode(**properties):
    return properties['value']


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

def IntNode(**properties):
    return properties['value']


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

def FloatNode(**properties):
    return properties['value']


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

def StringNode(**properties):
    return properties['value']
