import matplotlib
from matplotlib._pylab_helpers import Gcf
from matplotlib.backends.backend_agg import new_figure_manager, FigureCanvasAgg
from matplotlib.figure import Figure

import console
from io import BytesIO

def show(close=None, block=None):
    for figure_manager in Gcf.get_all_fig_managers():
        buf = BytesIO()
        figure_manager.canvas.print_png(buf)
        console.print_image(buf.getvalue())





FigureCanvas = FigureCanvasAgg
