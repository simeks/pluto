# -*- coding: utf-8 -*-

print('* Running Pluto startup script')

try:
    import pluto
except ImportError:
    print('Failed to import pluto')
else:
	print('Import module: pluto [Version: %s]' % pluto.version())
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
