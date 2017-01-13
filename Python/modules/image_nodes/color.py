import numpy as np
from flow import context_node
from image import Image

@context_node('SplitChannels', 'Image')
def color_split_channels(ctx):
    """
    Args:
        Image
    Returns:
        R
        G
        B
        A
    """
    if not ctx.is_pin_linked('Image'):
        return

    img = ctx.read_pin('Image')
    if type(img) != Image:
        raise TypeError('Expected Image')

    arr = img.array()
    dtype = str(arr.dtype)

    if (ctx.is_pin_linked('R')):
        ctx.write_pin('R', Image(arr[::,::,0], dtype))

    if (ctx.is_pin_linked('G')):
        ctx.write_pin('G', Image(arr[::,::,1], dtype))
    
    if (ctx.is_pin_linked('B')):
        ctx.write_pin('B', Image(arr[::,::,2], dtype))

    if (ctx.is_pin_linked('A')):
        ctx.write_pin('A', Image(arr[::,::,3], dtype))
