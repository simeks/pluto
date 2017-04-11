import flow
import numpy as np
import inspect
import re


class ScriptFunctionNode(flow.Node):
    def __init__(self, **kwargs):
        """
        Arguments:
            node_class  : String identifier of node, automatically required via func 
                          (func.__module__+'.'+func.__name__) if not set.
            title       : Title of the node. (required)
            category    : Category, set to '' if not set.
            pins        : Dict of flow.Pins specifying the pins of the node.
            properties  : Dict of properties.
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
        for pin in pins.keys():
            self.add_pin(pin, pins[pin])

        properties = kwargs.get('properties', [])
        for prop in properties.keys():
            self.add_property(prop, properties[prop])
        
        self.func = kwargs['func']
        self.signature = inspect.signature(self.func)

        ui = kwargs.get('ui', {})
        if 'ui_class' in ui:
            self.set_ui_class(ui['ui_class'])

    def run(self, ctx):
        if self.func == None:
            return
        
        # Here we map pins and properties to the function arguments.
        #     For instance: If we have a node with function signature fn(a, b),
        #     then we want to map any pins (or properties) named a or b to arguments
        #     when invoking the function.
        # Pins and properties are mapped according to this priority list:
        #     Given a function argument 'a':
        #     1. If we have a pin 'a' that is currently linked, pass value on pin 'a' as argument.
        #     2. Else if we have an property 'a', pass value of property 'a' as argument.
        #     3. Else if argument has a default value, pass that
        #     4. Else, pass None
        
        args = {}
        for arg in self.signature.parameters:
            if ctx.is_pin_linked(arg): # Check both if pin is exists and if linked
                args[arg] = ctx.read_pin(arg)
            elif self.property(arg):
                args[arg] = self.property(arg).value()
            else:
                if self.signature.parameters[arg].default == inspect._empty:
                    

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
    
