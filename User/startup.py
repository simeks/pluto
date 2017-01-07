# -*- coding: utf-8 -*-

print('* Running Pluto startup script')

try:
    import pluto
except ImportError:
    print('Failed to import pluto')
else:
    print('Import module: pluto [Version: %s]' % pluto.__version__)

__real_import__ = __import__

def pluto_import(name, globals=None, locals=None, fromlist=(), level=0):
    m = __real_import__(name, globals, locals, fromlist, level)
    pluto.auto_reload(m)
    return m

__builtins__.__import__ = pluto_import

try:
    import numpy as np
except ImportError:
    print('Failed to import numpy')
else:
    print('Import module: numpy as np [Version: %s]' % np.__version__)

try:
    import flow
except ImportError as e:
    print('Failed to import flow:',e)
else:
    print('Import module: flow')

try:
    import image
except ImportError as e:
    print('Failed to import image:',e)
else:
    print('Import module: image')

import console
import importlib
import os
import sys

for m in os.listdir(pluto.module_dir()):
    try:
        module = importlib.import_module(m)
        setattr(sys.modules[__name__], m, module)
    except ImportError as e:
        print('Failed to import %s:' % m, e)
    else:
        print('Import module: %s' % m)    

if image:
    Image = image.Image

if medkit:
    imread = medkit.read
    imwrite = medkit.write

# Load graphs
graphs_path = os.path.join(pluto.user_dir(), 'graphs')
if os.path.isdir(graphs_path):
    print('Installing graph nodes from (%s):' % graphs_path)
    for e in os.listdir(graphs_path):
        flow.install_graph_node_from_file(os.path.join(graphs_path, e))
        print(e)


os.chdir('../_sandbox')
w = flow.window()

