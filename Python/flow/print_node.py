from flow import Node, Pin, install_node_template

class PrintNode(Node):
    def __init__(self):
        super(PrintNode, self).__init__('print_node.PrintNode', 'Print', 'Flow')

        self.add_pin(Pin(Pin.In, 'In'))
        self.set_ui_class('print_node')

    def run(self, ctx):
        text = str(ctx.read_pin('In'))
        self.invoke_ui_method('print_text', text)
        print(text)

install_node_template(PrintNode())