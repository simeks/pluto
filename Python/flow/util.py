from flow import Node, Pin, FileProperty, node_template

import numpy as np

def eval_wrapper(code):
    if code != '':
        return eval(code)

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
    func=eval_wrapper
)

def file_browse(file):
    return file

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
    func=file_browse
)

