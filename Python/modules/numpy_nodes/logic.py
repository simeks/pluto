import numpy as np
from flow import node

@node('Equal', 'Image/Logic')
def equal(x1, x2):
    """
    Returns:
    Out
    """
    return np.equal(x1, x2)