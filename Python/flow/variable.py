from flow import Pin, node_template

# node_template(
#     title='Variable',
#     category='Flow',
#     ui={
#         ui_class = 'variable_node'
#     },
#     pins={
#         'Out': Pin(Pin.Out)
#     },
#     properties={
#         'name': '',
#         'default': ''
#     },
#     node_class = 'flow.variable.VariableNode',
#     func=variable_node
# )

# def variable_node(ctx):
#     if self.name in ctx.env:
#         ctx.write_pin('Out', ctx.env[self.name])
#     else:
#         ctx.write_pin('Out', self.default)

