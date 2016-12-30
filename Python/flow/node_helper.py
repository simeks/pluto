import flow
import numpy as np
import inspect
import re
from pluto import pluto_class

def parse_args(fn):
    return inspect.getargspec(fn).args

def parse_returns(fn):
    doc = fn.__doc__
    if doc == None:
        return []
    lines = [l.strip() for l in doc.split('\n')]
    begin = lines.index('Returns:')
    
    returns = []
    for i in range(begin+1, len(lines)):
        if lines[i] == '':
            break
        m = re.match('(\S)', lines[i])
        if m:
            returns.append(m.group(1))

    return returns


@pluto_class
class FunctionNode(flow.Node):
    def __init__(self, fn, title=None, category=None):
        super(FunctionNode, self).__init__()
        if not hasattr(fn, '__name__'):
            raise TypeError('Invalid function type, make sure you use the different @node decorators.')

        self.args = parse_args(fn)
        if self.args:
            for a in self.args:
                self.add_pin(a, flow.Pin.In)

        self.returns = parse_returns(fn)
        for a in self.returns:
            self.add_pin(a, flow.Pin.Out)

        self.func = fn
        self.node_class = fn.__module__ + '.' + fn.__name__

        if title != None:
            self.title = title
        if category != None:
            self.category = category

    def run(self, ctx):
        if self.func == None:
            return
        args = []
        for a in self.args:
            args.append(ctx.read_pin(a))
        
        returns = self.func(*args)

        if returns:
            if len(returns) != len(self.returns):
                raise ValueError('Not enough return arguments')

            for i in range(0, len(returns)):
                ctx.write_pin(self.returns[i], returns[i])


def node(title, category = ''):
    def dec(fn):
        flow.install_node_template(FunctionNode(fn=fn, title=title, category=category))
        return fn
    return dec


@node('node_a', 'test')
def node_a(A, B):
    """
    Returns:
    A -- asd
    B -- asd
    """
    return A, B

@node('print_a', 'test')
def print_a(A):
    print('A:',A)

@node('print_b', 'test')
def print_b(B):
    print('B:',B)