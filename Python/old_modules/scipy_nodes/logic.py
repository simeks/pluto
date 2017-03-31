import numpy as np

from .helper import register_node

register_node(np.all, "all", "NumPy/Logic/Truth value testing")
register_node(np.any, "any", "NumPy/Logic/Truth value testing")

register_node(np.isfinite, "isfinite", "NumPy/Logic/Array contents")
register_node(np.isinf, "isinf", "NumPy/Logic/Array contents")
register_node(np.isnan, "isnan", "NumPy/Logic/Array contents")
register_node(np.isneginf, "isneginf", "NumPy/Logic/Array contents")
register_node(np.isposinf, "isposinf", "NumPy/Logic/Array contents")

register_node(np.iscomplex, "iscomplex", "NumPy/Logic/Array type testing")
register_node(np.iscomplexobj, "iscomplexobj", "NumPy/Logic/Array type testing")
register_node(np.isfortran, "isfortran", "NumPy/Logic/Array type testing")
register_node(np.isreal, "isreal", "NumPy/Logic/Array type testing")
register_node(np.isrealobj, "isrealobj", "NumPy/Logic/Array type testing")
register_node(np.isscalar, "isscalar", "NumPy/Logic/Array type testing")

register_node(np.logical_and, "logical_and", "NumPy/Logic/Logical operations")
register_node(np.logical_or, "logical_or", "NumPy/Logic/Logical operations")
register_node(np.logical_not, "logical_not", "NumPy/Logic/Logical operations")
register_node(np.logical_xor, "logical_xor", "NumPy/Logic/Logical operations")

register_node(np.allclose, "allclose", "NumPy/Logic/Comparison")
register_node(np.isclose, "isclose", "NumPy/Logic/Comparison")
register_node(np.array_equal, "array_equal", "NumPy/Logic/Comparison")
register_node(np.array_equiv, "array_equiv", "NumPy/Logic/Comparison")
register_node(np.greater, "greater", "NumPy/Logic/Comparison")
register_node(np.greater_equal, "greater_equal", "NumPy/Logic/Comparison")
register_node(np.less, "less", "NumPy/Logic/Comparison")
register_node(np.less_equal, "less_equal", "NumPy/Logic/Comparison")
register_node(np.equal, "equal", "NumPy/Logic/Comparison")
register_node(np.not_equal, "not_equal", "NumPy/Logic/Comparison")
