import numpy as np
from flow import node
from image import Image

from registration import transform

@node('Transform', 'Registration')
def transform_node(image, deformation):
    """
    Args:
        Image (Image) : Image to transform
        Deformation (Image) : Deformation field to apply
    
    Returns:
        Out (Image) : Resulting image
    
    """

    return transform(image, deformation)