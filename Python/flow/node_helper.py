import flow
import numpy as np
import inspect
import re
from pluto import pluto_class

def parse_args_from_fn(fn):
    return inspect.getargspec(fn).args

def parse_returns(fn):
    """
    Looks for returns in the given functions docstring.
    
    For this function to be successful the docstring is required according to: 
    http://sphinxcontrib-napoleon.readthedocs.io/en/latest/example_google.html
    """

    doc = fn.__doc__
    if doc == None:
        return []
    lines = doc.split('\n')
    begin = -1
    end = len(lines)
    for i, l in enumerate(lines):
        if re.match('( {4}|\t)Returns:', l):
            begin = i
        if begin != -1 and re.match('(( {4}|\t)(Arguments:)|(Args:))|(\w*$)', l):
            end = i

    if begin == -1:
        return []

    returns = []
    for i in range(begin+1, end):
        m = re.match('( {8}|\t\t)(\S+)', lines[i])
        if m:
            returns.append(m.group(2))

    return returns

def parse_args(fn):
    """
    Looks for arguments in the given functions docstring.
    
    For this function to be successful the docstring is required according to: 
    http://sphinxcontrib-napoleon.readthedocs.io/en/latest/example_google.html
    """

    doc = fn.__doc__
    if doc == None:
        return []
    lines = doc.split('\n')
    begin = -1
    end = len(lines)
    for i, l in enumerate(lines):
        if re.match('( {4}|\t)(Arguments:)|(Args:):', l):
            begin = i
        if begin != -1 and re.match('(( {4}|\t)Returns:)|(\w*$)', l):
            end = i

    if begin == -1:
        return []

    returns = []
    for i in range(begin+1, end):
        m = re.match('( {8}|\t\t)(\S+)', lines[i])
        if m:
            returns.append(m.group(2))

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

