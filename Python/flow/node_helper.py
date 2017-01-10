import flow
import numpy as np
import inspect
import re
from pluto import pluto_class

def parse_args_from_fn(fn):
    return inspect.getargspec(fn).args

def parse_returns(fn):
    doc = fn.__doc__
    if doc == None:
        return []
    lines = [l.strip() for l in doc.split('\n')]
    begin = lines.index('Returns:')
    
    returns = []
    for i in range(begin+1, len(lines)):
        if lines[i] == '' or lines[i].startswith('Arguments:'):
            break
        m = re.match('(\S+)', lines[i])
        if m:
            returns.append(m.group(1))

    return returns

def parse_args(fn):
    doc = fn.__doc__
    if doc == None:
        return []
    lines = [l.strip() for l in doc.split('\n')]
    begin = lines.index('Arguments:')
    
    args = []
    for i in range(begin+1, len(lines)):
        if lines[i] == '' or lines[i].startswith('Returns:'):
            break
        m = re.match('(\S+)', lines[i])
        if m:
            args.append(m.group(1))

    return args

@pluto_class
class FunctionNode(flow.Node):
    def __init__(self, fn, title=None, category=None):
        super(FunctionNode, self).__init__()

        self.args = parse_args_from_fn(fn)
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
        if returns is not None:
            if len(self.returns) > 1:
                if len(returns) != len(self.returns):
                    raise ValueError('Not enough return arguments')

                for i in range(0, len(returns)):
                    ctx.write_pin(self.returns[i], returns[i])
            else:
                ctx.write_pin(self.returns[0], returns)


@pluto_class
class ContextFunctionNode(flow.Node):
    def __init__(self, fn, title=None, category=None):
        super(ContextFunctionNode, self).__init__()

        self.args = parse_args(fn)
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
        self.func(ctx)


def node(title, category = ''):
    def dec(fn):
        flow.install_node_template(FunctionNode(fn=fn, title=title, category=category))
        return fn
    return dec

def context_node(title, category = ''):
    def dec(fn):
        flow.install_node_template(ContextFunctionNode(fn=fn, title=title, category=category))
        return fn
    return dec

