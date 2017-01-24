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


class NumpyDocstringParse(object):
    """
    Parses numpy-style docstring.

    Parameters
    ----------

    Returns
    -------
        x : type 
            Description

    """

    field_regex = re.compile(r'\s*:')

    def __init__(self, docstring):
        self.docstring = docstring.splitlines()
        self.docstring = [l.rstrip() for l in self.docstring]
        self.iter = Iter(self.docstring)
        self.inputs = []
        self.outputs = []
        self.sections = {'parameters': self.parse_inputs, 'returns': self.parse_outputs}
        self.parse()

    def line_indent(self, line):
        return len(line) - len(line.lstrip())

    def parse(self):
        while not self.iter.end():
            line = self.iter.next().strip().strip(':')
            if line.lower() in self.sections:
                self.iter.next() # Skip line under section title
                self.sections[line.lower()]()

    def parse_parameter(self):
        line = self.iter.next()
        indent = self.line_indent(line)

        t = line.split(':')
        t2 = t[0].split(',')

        names = [n.strip() for n in t2]

        # Parse multi-line description
        next_line = self.iter.peek()
        while self.line_indent(next_line) > indent:
            self.iter.next()
            next_line = self.iter.peek()
            
        return names

    def parse_return(self, i):
        line = self.iter.next()
        indent = self.line_indent(line)

        if ':' in line:
            t = line.split(':')
            t2 = t[0].split(',')
            names = [n.strip() for n in t2]
        else:
            # No name on return
            names = ['Out'+str(i)]


        # Parse multi-line description
        next_line = self.iter.peek()
        while self.line_indent(next_line) > indent:
            self.iter.next()
            next_line = self.iter.peek()
            
        return names

    def parse_parameters(self):
        ret = []
        while not self.section_end():
            name = self.parse_parameter()
            ret.extend(name)
        return ret

    def parse_returns(self):
        ret = []
        i = 0
        while not self.section_end():
            name = self.parse_return(i)
            ret.extend(name)
            i += 1
        return ret

    def parse_inputs(self):
        self.inputs.extend(self.parse_parameters())

    def parse_outputs(self):
        self.outputs.extend(self.parse_returns())

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
class NumpyNode(flow.Node):
    def __init__(self, fn, title=None, category=None):
        super(NumpyNode, self).__init__()

        if fn.__doc__ is not None:
            ndoc = NumpyDocstringParse(fn.__doc__)

            self.args = ndoc.inputs
            for i in self.args:
                self.add_pin(i, flow.Pin.In)

            self.returns = ndoc.outputs
            for o in self.returns:
                self.add_pin(o, flow.Pin.Out)

        self.func = fn
        self.node_class = 'numpy' + '.' + fn.__name__

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


def register_node(fn, title, category):
    flow.install_node_template(NumpyNode(fn, title, category))
