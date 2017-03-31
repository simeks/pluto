import numpy as np
from flow import node, node_template, Pin
import image
from .types import *

def magnitude(img):
    if type(img) != image.Image:
        raise TypeError('Expected Image')

    n_comp = img.shape[-1]
    shape = img.shape[0:-1]
    img = img.reshape((np.product(shape), n_comp))
    m = np.zeros(np.product(shape))
    for i in range(0, n_comp):
        m = np.add(m, img[:,i]*img[:,i])
    
    m = np.sqrt(m)
    out = image.Image(m.reshape(shape).astype('float32'), PixelType_Float32)
    if hasattr(img, 'spacing'):
        out.spacing = img.spacing
    if hasattr(img, 'origin'):
        out.origin = img.origin
    return out
    

node_template(
    title='Magnitude',
    category='Image/Vectorfield',
    pins={
        'Img': Pin(Pin.In),
        'Out': Pin(Pin.Out)
    },
    func=magnitude
)
