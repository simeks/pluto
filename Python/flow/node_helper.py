from flow import Node, Pin, Property, install_node_template
import numpy as np
import inspect
import re


class ScriptFunctionNode(Node):
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

        self.func = kwargs['func']
        self.signature = inspect.signature(self.func)

        node_class = kwargs.get('node_class', self.func.__module__+'.'+self.func.__name__)
        title = kwargs['title']
        category = kwargs.get('category', '')

        super(ScriptFunctionNode, self).__init__(node_class, title, category)

        self.out_pins = []

        pins = kwargs.get('pins', {})
        for pin in pins.keys():
            # Set name according to the dict defined by the user
            pins[pin].set_name(pin)
            self.add_pin(pins[pin])
            if pins[pin].pin_type() == Pin.Out:
                self.out_pins.append(pin)

        properties = kwargs.get('properties', {})
        for key in properties.keys():
            prop = properties[key]
            if not isinstance(prop, Property):
                prop = Property(prop)
            prop.set_name(key)
            self.add_property(prop)

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
            else:
                p = self.property(arg)
                if p:
                    args[arg] = p.value()
                else:
                    if self.signature.parameters[arg].default == inspect._empty:
                        args[arg] = None

        returns = self.func(**args)

        # For return values:
        #   1. If function returns a dict, map each entry in dict to an output pin
        #   2. Else if function returns a tuple, 
        #       2.1 if we only have one pin, write tuple to pin
        #       2.2 else map tuple to pins
        #   3. Else if function returns a single value, write value to pin (if any)
        #   4. Else if function returns None, write None we have an existing pin, or do nothing.

        if type(returns) is dict:
            for key in returns:
                if ctx.is_pin_linked(key):
                    ctx.write_pin(key, returns[key])
        elif type(returns) is tuple:
            if len(self.out_pins) == 1:
                ctx.write_pin(self.out_pins[0])
            elif len(self.out_pins) == 0:
                print('[Warning] Node missing output pins')
            else:
                for i in range(0, min(len(self.out_pins), len(returns))):
                    ctx.write_pin(self.out_pins[i], returns[i])
        elif returns is not None:
            if len(self.out_pins) > 0:
                ctx.write_pin(self.out_pins[0], returns)
            else:
                print('[Warning] Node missing output pins')
        else:
            if len(self.out_pins) > 0:
                ctx.write_pin(self.out_pins[0], None)


def node_template(**kwargs):
    install_node_template(ScriptFunctionNode(**kwargs))
    
