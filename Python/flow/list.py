from flow import ArrayPin, Pin, node_template
import re


def concatenate(lst):
    out = []
    for l in lst:
        out.extend(l)
    return out

def index(lst, index):
    """
    Returns the item(s) in the specified index of a list.
    Supports slicing (e.g. list[1:5])
    """
    index = index.strip()
    if not re.match('[\-0-9:]+$', index):
        raise ValueError('Invalid index')

    return eval('lst[%s]' % index)


node_template(
    title='Build',
    category='List',
    pins={
        'Item': ArrayPin(Pin.In),
        'Out': Pin(Pin.Out)
    },
    doc='Combines all input items to a list',
    node_class = 'flow.list.BuildNode',
    func=lambda item : item # Items should already be a list by now
)

node_template(
    title='Concatenate',
    category='List',
    pins={
        'Item': ArrayPin(Pin.In),
        'Out': Pin(Pin.Out)
    },
    doc='Concatenates all input lists to a single list',
    node_class = 'flow.list.ConcatenateNode',
    func=concatenate
)

node_template(
    title='Index',
    category='List',
    ui={
        'ui_class': 'one_to_one_node',
        'ui_node_title_var': 'index'
    },
    properties={
        'index': '0'
    },
    pins={
        'In': Pin(Pin.In),
        'Out': Pin(Pin.Out)
    },
    node_class = 'flow.list.IndexNode',
    func=index
)


