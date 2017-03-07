from flow import node_template, Pin, BoolProperty, IntProperty, FloatProperty, StringProperty


node_template(
    title='Bool',
    category='Constants',
    properties={
        'value': BoolProperty(False)
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    doc='Boolean constant',
    func=BoolNode
)

def BoolNode(ctx):
    ctx.write_pin('Out', ctx.property('value'))


node_template(
    title='Integer',
    category='Constants',
    properties={
        'value': IntProperty(0)
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    doc='Integer constant',
    func=IntNode
)

def IntNode(ctx):
    ctx.write_pin('Out', ctx.property('value'))


node_template(
    title='Float',
    category='Constants',
    properties={
        'value': FloatProperty(0.0)
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    doc='Float constant',
    func=FloatNode
)

def FloatNode(ctx):
    ctx.write_pin('Out', ctx.property('value'))


node_template(
    title='String',
    category='Constants',
    properties={
        'value': StringProperty('')
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    doc='String constant',
    func=StringNode
)

def StringNode(ctx):
    ctx.write_pin('Out', ctx.property('value'))
