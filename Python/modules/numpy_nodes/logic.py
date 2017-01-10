import numpy as np
from flow import node

@node('Equal', 'NumPy/Logic')
def equal(x1, x2):
    """
    Returns:
    Out
    """
    return np.equal(x1, x2)

@node('And', 'NumPy/Logic')
def logical_and(x1, x2):
    """
    Returns:
    Out
    """
    return np.logical_and(x1, x2)
