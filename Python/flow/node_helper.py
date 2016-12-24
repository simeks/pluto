import flow
import numpy as np
import inspect

class FunctionNode(flow.Node):
    def __init__(self, func=None, args=None, title=None, category=None):
        super(FunctionNode, self).__init__()
        if not hasattr(func, '__name__'):
            raise TypeError('Invalid function type, make sure you use the different @node_func decorators.')

        if args:
            self.args = args
        else:
            self.args = []

        for a in self.args:
            self.add_pin(a, flow.Pin.In)
        self.add_pin('Out', flow.Pin.Out)

        self.func = func
        self.node_class = func.__module__ + '.' + func.__name__

        if title != None:
            self.title = title
        if category != None:
            self.category = category

    def run(self, ctx):
        if self.func == None:
            return
        print('wowowowowow')

def node_func(title, category = ''):
    def dec(fn):
        flow.install_node_template(FunctionNode(func=fn, args=inspect.getargspec(fn).args, title=title, category=category))
        return fn
    return dec