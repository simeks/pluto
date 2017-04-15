from flow import Node, Pin, install_node_template

class PrintNode(Node):
    def __init__(self):
        super(PrintNode, self).__init__('print_node.PrintNode', 'Print', 'Flow')

        self.add_pin(Pin(Pin.In, 'In'))
        self.set_ui_class('print_node')

        self.text = ''

    def run(self, ctx):
        self.text = str(ctx.read_pin('In'))
        print(self.text)

install_node_template(PrintNode())