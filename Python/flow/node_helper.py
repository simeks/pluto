import flow
import numpy as np
import inspect
import re


class ScriptFunctionNode(flow.Node):
    def __init__(self, **kwargs):
        """
        Arguments:
            title       : Title of the node. (required)
            category    : Category, set to '' if not set.
            node_class  : String identifier of node, automatically required via func 
                          (func.__module__+'.'+func.__name__) if not set.
            pins        : List of flow.Pins specifying the pins of the node.
            properties  : List of properties.
            ui          : Tuple of UI options.
            func        : Function bound to node. (required)
        """

        if 'title' not in kwargs:
            raise ValueError('\'title\' missing')

        if 'func' not in kwargs:
            raise ValueError('\'func\' missing')

        node_class = kwargs.get('node_class', func.__module__+'.'+func.__name__)
        title = kwargs['title']
        category = kwargs.get('category', '')

        super(ScriptFunctionNode, self).__init__(node_class, title, category)

        pins = kwargs.get('pins', [])
        for pin in pins:
            self.add_pin(

        func = kwargs['func']
        properties = kwargs.get('properties', [])
        
        ui = kwargs.get('ui', ())

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

    def create_pin_to_arg_mapping(self):
        pass

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


def node_template(**kwargs):
    flow.install_node_template(ScriptFunctionNode(**kwargs))
    
