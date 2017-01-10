from flow import node

@node('Eval', 'Python')
def eval_node(code):
    """
    Returns:
    Out
    """
    return eval(code)