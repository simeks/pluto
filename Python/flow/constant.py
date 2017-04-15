from flow import node_template, Pin


node_template(
    title='Bool',
    category='Constants',
    node_class='constant.Bool',
    properties={
        'value': False
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    doc='Boolean constant',
    func=lambda value : bool(value)
)

node_template(
    title='Integer',
    category='Constants',
    node_class='constant.Integer',
    properties={
        'value': 0
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    doc='Integer constant',
    func=lambda value : int(value)
)

node_template(
    title='Float',
    category='Constants',
    node_class='constant.Float',
    properties={
        'value': 0.0
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    doc='Float constant',
    func=lambda value : float(value)
)

node_template(
    title='String',
    category='Constants',
    node_class='constant.String',
    properties={
        'value': ''
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    doc='String constant',
    func=lambda value : str(value)
)

