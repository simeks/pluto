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
user_graphs_path = os.path.join(pluto.user_dir(), 'graphs')
if os.path.isdir(user_graphs_path):
    print('Installing graph nodes from (%s):' % user_graphs_path)
    for root, _, files in os.walk(user_graphs_path):
        for f in files:
            if f.endswith('.flow'):
                local_path = root[len(user_graphs_path)+1:]
                cls = 'graph:' + local_path.replace(os.sep, '.') + '.' + os.path.splitext(f)[0]
                tpl = flow.install_graph_node_from_file(cls, os.path.join(root, f))

                tpl.title = f[0].upper() + os.path.splitext(f)[0][1:]
                cat = 'Graphs/'
                for t in local_path.split(os.sep):
                    cat += t[0].upper() + t[1:] + '/'
                if cat != '':
                    cat = cat[0:len(cat)-1]
                tpl.category = cat
                print(os.path.join(root, f)+', Class:',cls)



os.chdir('../_sandbox')
w = flow.window()

