import numpy as np

from .helper import register_node

register_node(np.empty, 'empty', 'NumPy/Array/Ones and zeros')
register_node(np.empty_like, 'empty_like', 'NumPy/Array/Ones and zeros')
register_node(np.eye, 'eye', 'NumPy/Array/Ones and zeros')
register_node(np.identity, 'identity', 'NumPy/Array/Ones and zeros')
register_node(np.ones, 'ones', 'NumPy/Array/Ones and zeros')
register_node(np.ones_like, 'ones_like', 'NumPy/Array/Ones and zeros')
register_node(np.zeros, 'zeros', 'NumPy/Array/Ones and zeros')
register_node(np.zeros_like, 'zeros_like', 'NumPy/Array/Ones and zeros')
register_node(np.full, 'full', 'NumPy/Array/Ones and zeros')
register_node(np.full_like, 'full_like', 'NumPy/Array/Ones and zeros')

register_node(np.arange, 'arange', 'NumPy/Array/Numerical ranges')
register_node(np.linspace, 'linspace', 'NumPy/Array/Numerical ranges')
register_node(np.logspace, 'logspace', 'NumPy/Array/Numerical ranges')
#register_node(np.geomspace, 'geomspace', 'NumPy/Array/Numerical ranges')


# Changing array shape
register_node(np.reshape, 'reshape', 'NumPy/Array/Shape')
register_node(np.ravel, 'ravel', 'NumPy/Array/Shape')
register_node(np.ndarray.flatten, 'flatten', 'NumPy/Array/Shape')

# Transpose-like operations
register_node(np.moveaxis, 'moveaxis', 'NumPy/Array/Transpose')
register_node(np.rollaxis, 'rollaxis', 'NumPy/Array/Transpose')
register_node(np.swapaxes, 'swapaxes', 'NumPy/Array/Transpose')
register_node(np.transpose, 'transpose', 'NumPy/Array/Transpose')

# Changing number of dimensions
register_node(np.atleast_1d, 'atleast_1d', 'NumPy/Array/Dimensions')
register_node(np.atleast_2d, 'atleast_2d', 'NumPy/Array/Dimensions')
register_node(np.atleast_3d, 'atleast_3d', 'NumPy/Array/Dimensions')
register_node(np.broadcast_to, 'broadcast_to', 'NumPy/Array/Dimensions')
register_node(np.broadcast_arrays, 'broadcast_arrays', 'NumPy/Array/Dimensions')
register_node(np.expand_dims, 'expand_dims', 'NumPy/Array/Dimensions')
register_node(np.squeeze, 'squeeze', 'NumPy/Array/Dimensions')

# Joining arrays
register_node(np.concatenate, 'concatenate', 'NumPy/Array/Joining')
register_node(np.stack, 'stack', 'NumPy/Array/Joining')
register_node(np.column_stack, 'column_stack', 'NumPy/Array/Joining')
register_node(np.dstack, 'dstack', 'NumPy/Array/Joining')
register_node(np.hstack, 'hstack', 'NumPy/Array/Joining')
register_node(np.vstack, 'vstack', 'NumPy/Array/Joining')

# Splitting arrays
register_node(np.split, 'split', 'NumPy/Array/Splitting')
#register_node(np.array_split, 'array_split', 'NumPy/Array/Splitting')
#register_node(np.dsplit, 'dsplit', 'NumPy/Array/Splitting')
#register_node(np.hsplit, 'hsplit', 'NumPy/Array/Splitting')
#register_node(np.vsplit, 'vsplit', 'NumPy/Array/Splitting')

# Tiling arrays
register_node(np.tile, 'tile', 'NumPy/Array/Tiling')
register_node(np.repeat, 'repeat', 'NumPy/Array/Tiling')

# Adding and removing elements
register_node(np.delete, 'delete', 'NumPy/Array/Elements')
register_node(np.insert, 'insert', 'NumPy/Array/Elements')
register_node(np.append, 'append', 'NumPy/Array/Elements')
register_node(np.resize, 'resize', 'NumPy/Array/Elements')
register_node(np.trim_zeros, 'trim_zeros', 'NumPy/Array/Elements')
register_node(np.unique, 'unique', 'NumPy/Array/Elements')

# Rearranging elements
#register_node(np.flip, 'flip', 'NumPy/Array/Rearranging')
register_node(np.fliplr, 'fliplr', 'NumPy/Array/Rearranging')
register_node(np.flipud, 'flipud', 'NumPy/Array/Rearranging')
register_node(np.reshape, 'reshape', 'NumPy/Array/Rearranging')
register_node(np.roll, 'roll', 'NumPy/Array/Rearranging')
register_node(np.rot90, 'rot90', 'NumPy/Array/Rearranging')
