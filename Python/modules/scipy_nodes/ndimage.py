from .helper import register_node

import scipy.ndimage

# Filters
import scipy.ndimage.filters

register_node(scipy.ndimage.filters.convolve, 'convolve', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.convolve1d, 'convolve1d', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.correlate, 'correlate', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.correlate1d, 'correlate1d', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.gaussian_filter, 'gaussian_filter', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.gaussian_filter1d, 'gaussian_filter1d', 'SciPy/Image processing/Filters')

register_node(scipy.ndimage.filters.gaussian_gradient_magnitude, 'gaussian_gradient_magnitude', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.gaussian_laplace, 'gaussian_laplace', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.generic_filter, 'generic_filter', 'SciPy/Image processing/Filters') 
register_node(scipy.ndimage.filters.generic_filter1d, 'generic_filter1d', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.generic_gradient_magnitude, 'generic_gradient_magnitude', 'SciPy/Image processing/Filters') 
register_node(scipy.ndimage.filters.generic_laplace, 'generic_laplace', 'SciPy/Image processing/Filters') 
register_node(scipy.ndimage.filters.laplace, 'laplace', 'SciPy/Image processing/Filters') 
register_node(scipy.ndimage.filters.maximum_filter, 'maximum_filter', 'SciPy/Image processing/Filters') 
register_node(scipy.ndimage.filters.maximum_filter1d, 'maximum_filter1d', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.median_filter, 'median_filter', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.minimum_filter, 'minimum_filter', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.minimum_filter1d, 'minimum_filter1d', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.percentile_filter, 'percentile_filter', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.prewitt, 'prewitt', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.rank_filter, 'rank_filter', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.sobel, 'sobel', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.uniform_filter, 'uniform_filter', 'SciPy/Image processing/Filters')
register_node(scipy.ndimage.filters.uniform_filter1d, 'uniform_filter1d', 'SciPy/Image processing/Filters')

# Fourier filters scipy.ndimage.fourier
import scipy.ndimage.fourier

register_node(scipy.ndimage.fourier.fourier_ellipsoid, 'fourier_ellipsoid', 'SciPy/Image processing/Fourier')
register_node(scipy.ndimage.fourier.fourier_gaussian, 'fourier_gaussian', 'SciPy/Image processing/Fourier')
register_node(scipy.ndimage.fourier.fourier_shift, 'fourier_shift', 'SciPy/Image processing/Fourier')
register_node(scipy.ndimage.fourier.fourier_uniform, 'fourier_uniform', 'SciPy/Image processing/Fourier')

# Interpolation scipy.ndimage.interpolation
import scipy.ndimage.interpolation

register_node(scipy.ndimage.interpolation.affine_transform, 'affine_transform', 'SciPy/Image processing/Interpolation')
register_node(scipy.ndimage.interpolation.geometric_transform, 'geometric_transform', 'SciPy/Image processing/Interpolation')
register_node(scipy.ndimage.interpolation.map_coordinates, 'map_coordinates', 'SciPy/Image processing/Interpolation')
register_node(scipy.ndimage.interpolation.rotate, 'rotate', 'SciPy/Image processing/Interpolation')
register_node(scipy.ndimage.interpolation.shift, 'shift', 'SciPy/Image processing/Interpolation')
register_node(scipy.ndimage.interpolation.spline_filter, 'spline_filter', 'SciPy/Image processing/Interpolation')
register_node(scipy.ndimage.interpolation.spline_filter1d, 'spline_filter1d', 'SciPy/Image processing/Interpolation')
register_node(scipy.ndimage.interpolation.zoom, 'zoom', 'SciPy/Image processing/Interpolation')

# Measurements scipy.ndimage.measurements
import scipy.ndimage.measurements

register_node(scipy.ndimage.measurements.center_of_mass, 'center_of_mass', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.extrema, 'extrema', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.find_objects, 'find_objects', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.histogram, 'histogram', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.label, 'label', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.labeled_comprehension, 'labeled_comprehension', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.maximum, 'maximum', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.maximum_position, 'maximum_position', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.mean, 'mean', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.minimum, 'minimum', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.minimum_position, 'minimum_position', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.standard_deviation, 'standard_deviation', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.sum, 'sum', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.variance, 'variance', 'SciPy/Image processing/Measurements')
register_node(scipy.ndimage.measurements.watershed_ift, 'watershed_ift', 'SciPy/Image processing/Measurements')

# Morphology scipy.ndimage.morphology
import scipy.ndimage.morphology

register_node(scipy.ndimage.morphology.binary_closing, 'binary_closing', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.binary_dilation, 'binary_dilation', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.binary_erosion, 'binary_erosion', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.binary_fill_holes, 'binary_fill_holes', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.binary_hit_or_miss, 'binary_hit_or_miss', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.binary_opening, 'binary_opening', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.binary_propagation, 'binary_propagation', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.black_tophat, 'black_tophat', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.distance_transform_bf, 'distance_transform_bf', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.distance_transform_cdt, 'distance_transform_cdt', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.distance_transform_edt, 'distance_transform_edt', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.generate_binary_structure, 'generate_binary_structure', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.grey_closing, 'grey_closing', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.grey_dilation, 'grey_dilation', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.grey_erosion, 'grey_erosion', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.grey_opening, 'grey_opening', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.iterate_structure, 'iterate_structure', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.morphological_gradient, 'morphological_gradient', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.morphological_laplace, 'morphological_laplace', 'SciPy/Image processing/Morphology')
register_node(scipy.ndimage.morphology.white_tophat, 'white_tophat', 'SciPy/Image processing/Morphology')

# Utility
register_node(scipy.ndimage.imread, 'imread', 'SciPy/Image processing/Utility')