from flow import Node, Pin, FileProperty, node_template

import numpy as np

node_template(
    title='Eval',
    category='Utilities',
    ui={
        'ui_class': 'single_pin_node'
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    properties={
        'code': ''
    },
    node_class = 'flow.util.Eval',
    func=eval
)

node_template(
    title='FileBrowse',
    category='Utilities',
    ui={
        'ui_class': 'single_pin_node'
    },
    pins={
        'Out': Pin(Pin.Out)
    },
    properties={
        'file': FileProperty('')
    },
    node_class = 'flow.util.FileBrowse',
    func=eval
)
def file_browse(file):
    return file

