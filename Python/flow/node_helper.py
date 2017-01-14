import flow
import numpy as np
import inspect
import re
from pluto import pluto_class

class Iter(object):
    def __init__(self, lst):
        self.lst = lst
        self.idx = 0

    def next(self):
        ret = self.lst[self.idx]
        self.idx += 1
        return ret

    def peek(self):
        return self.lst[self.idx]

    def end(self):
        return (self.idx >= len(self.lst))


class NodeDocstringParse(object):
    """
    Parses docstring for a node function.

    Node docstrings are expected to follow this structure:
    Args:
        InputPinA : Some comment
        InputPinB(type) : Inputs may also specify an expected type

    Returns:
        OutputPinA(str) : Output pin

    """

    field_regex = re.compile(r'\s*(\w+)')

    def __init__(self, docstring):
        self.docstring = docstring.splitlines()
        self.docstring = [l.rstrip() for l in self.docstring]
        self.iter = Iter(self.docstring)
        self.inputs = []
        self.outputs = []
        self.sections = {'args': self.parse_inputs, 'returns': self.parse_outputs}
        self.parse()

    def line_indent(self, line):
        return len(line) - len(line.lstrip())

    def parse(self):
        while not self.iter.end():
            line = self.iter.next().strip().strip(':')
            if line.lower() in self.sections:
                self.sections[line.lower()]()

    def parse_field(self):
        line = self.iter.next()
        indent = self.line_indent(line)

        name = None
        m = self.field_regex.match(line)
        if m:
            name = m.group(1)

        # Parse multi-line description
        next_line = self.iter.peek()
        while self.line_indent(next_line) > indent:
            self.iter.next()
            next_line = self.iter.peek()
            
        return name

    def parse_fields(self):
        ret = []
        while not self.section_end():
            name = self.parse_field()
            ret.append(name)
        return ret

    def parse_inputs(self):
        self.inputs.extend(self.parse_fields())

    def parse_outputs(self):
        self.outputs.extend(self.parse_fields())

    def section_end(self):
        if self.iter.end():
            return True

        l = self.iter.peek().strip()
        if l == '':
            return True
        elif l.strip(':').lower() in self.sections:
            return True
        return False 


@pluto_class
class FunctionNode(flow.Node):
    def __init__(self, fn, title=None, category=None):
        super(FunctionNode, self).__init__()

        self.args = inspect.getargspec(fn).args
        if self.args is not None:
            for i in self.args:
                self.add_pin(i, flow.Pin.In)

        if fn.__doc__ is not None:
            ndoc = NodeDocstringParse(fn.__doc__)

            self.returns = ndoc.outputs
            for o in self.returns:
                self.add_pin(o, flow.Pin.Out)

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
                if len(returns) > len(self.returns):
                    raise ValueError('To many return arguments')

                for i in range(0, len(returns)):
                    ctx.write_pin(self.returns[i], returns[i])
            else:
                ctx.write_pin(self.returns[0], returns)


@pluto_class
class ContextFunctionNode(flow.Node):
    def __init__(self, fn, title=None, category=None):
        super(ContextFunctionNode, self).__init__()

        if fn.__doc__ is not None:
            ndoc = NodeDocstringParse(fn.__doc__)
            for i in ndoc.inputs:
                self.add_pin(i, flow.Pin.In)
            for o in ndoc.outputs:
                self.add_pin(o, flow.Pin.Out)

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

