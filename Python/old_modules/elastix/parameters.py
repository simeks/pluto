from flow import node

class Parameters:
    def __init__(self, file):
        with open(file, 'r') as f:
            self.txt = f.read()

    def write(self, file):
        with open(file, 'w') as f:
            f.write(self.txt)

@node('ParameterRead', 'Elastix')
def read(File):
    """
    Returns:
        Params
    """
    if type(File) != str:
        raise ValueError('Unexpected arguments')

    return Parameters(File)


@node('ParameterWrite', 'Elastix')
def write(Params, File):
    if type(Params) != Parameters or type(File) != str:
        raise ValueError('Unexpected arguments')

    Params.write(File)

