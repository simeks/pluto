from __future__ import (absolute_import, division, print_function,
                        unicode_literals)

from matplotlib.externals import six

import matplotlib
from matplotlib._pylab_helpers import Gcf
from matplotlib.backends.backend_agg import new_figure_manager, FigureCanvasAgg
from matplotlib.figure import Figure
from matplotlib.transforms import Bbox

def show(close=None, block=None):
    pass




FigureCanvas = FigureCanvasAgg
