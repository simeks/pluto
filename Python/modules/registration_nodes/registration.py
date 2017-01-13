import numpy as np
from flow import context_node
from image import Image

from registration import RegistrationEngine

@context_node('Registration', 'Registration')
def run_registration(ctx):
    """
    Args:
        Fixed0
        Moving0

        Fixed1
        Moving1
        
        Fixed2
        Moving2
        
        ConstraintMask
        ConstraintValues
        
        StartingGuess
    
    Returns:
        Deformation
    
    """
    pass
    #eng = RegistrationEngine('blocked_graph_cut', image.PixelType_Float32)