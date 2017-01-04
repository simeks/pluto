# -*- coding: utf-8 -*-

print('* Running Pluto startup script')

import sys
sys.path.append('../Python')

try:
    import pluto
except ImportError:
    print('Failed to import pluto')
else:
    print('Import module: pluto [Version: %s]' % pluto.__version__)
try:
    import numpy as np
except ImportError:
    print('Failed to import numpy')
else:
    print('Import module: numpy as np [Version: %s]' % np.__version__)

try:
    import image
except ImportError:
    print('Failed to import image')
else:
    print('Import module: image')

from image import Image

try:
    import flow
except ImportError as e:
    print('Failed to import flow:',e)
else:
    print('Import module: flow')

import console

try:
    import medkit
except ImportError as e:
    print('Failed to import medkit:',e)
else:
    print('Import module: medkit')
imread = medkit.read
imwrite = medkit.write

import os

# Load graphs
graphs_path = os.path.join(pluto.user_dir(), 'graphs')
if os.path.isdir(graphs_path):
    print('Installing graph nodes from (%s):' % graphs_path)
    for e in os.listdir(graphs_path):
        flow.install_graph_node_from_file(os.path.join(graphs_path, e))
        print(e)


os.chdir('../_sandbox')
w = flow.window()

