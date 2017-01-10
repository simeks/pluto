import flow
import image
import medkit
import os
import subprocess
from elastix.parameters import Parameters

from pluto import pluto_class


elastix_exe = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'elastix.exe')
def run_elastix(param_file, fixed_imgs, moving_imgs, out, fixed_mask=None, moving_mask=None, t0=None, fp=None, mp=None):
    args = [elastix_exe, '-p', param_file, '-out', out]
    for i, f in enumerate(fixed_imgs):
        if f:
            args.extend(['-f'+str(i), f])
    for i, m in enumerate(moving_imgs):
        if m:
            args.extend(['-m'+str(i), m])

    if fixed_mask != None:
        args.extend(['-fMask', fixed_mask])
    if moving_mask != None:
        args.extend(['-mMask', moving_mask])

    if t0 != None:
        args.extend(['-t0', t0])

    if fp != None:
        args.extend(['-fp', fp])

    if mp != None:
        args.extend(['-mp', mp])

    proc = subprocess.Popen(args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    proc.wait()


@pluto_class
class ElastixNode(flow.Node):
    pins = [
        flow.Pin('F0', flow.Pin.In),
        flow.Pin('M0', flow.Pin.In),
        flow.Pin('F1', flow.Pin.In),
        flow.Pin('M1', flow.Pin.In),
        flow.Pin('F2', flow.Pin.In),
        flow.Pin('M2', flow.Pin.In),
        flow.Pin('T0', flow.Pin.In),
        flow.Pin('FixedMask', flow.Pin.In),
        flow.Pin('MovingMask', flow.Pin.In),
        flow.Pin('FixedLandmarks', flow.Pin.In),
        flow.Pin('MovingLandmarks', flow.Pin.In),
        flow.Pin('Out', flow.Pin.Out),
        flow.Pin('Transform', flow.Pin.Out),
    ]
    properties = [
        flow.Property('param_file', '')
    ]

    def __init__(self):
        super(ElastixNode, self).__init__()
        self.node_class = 'elastix.Elastix'
        self.title = 'Elastix'
        self.category = 'Elastix'
        self.value = ''

    def run(self, ctx):
        fixed = []
        moving = []
        for i in range(0, 3):
            fixed.append(ctx.read_pin('F'+str(i)))
            moving.append(ctx.read_pin('M'+str(i)))

        fixed_mask = ctx.read_pin('FixedMask')
        moving_mask = ctx.read_pin('MovingMask')

        fp = ctx.read_pin('FixedLandmarks')
        mp = ctx.read_pin('MovingLandmarks')
        if (fp != None and type(fp) != str) or (mp != None and type(mp) != str):
            raise ValueError('Wrong values for landmarks, expected string')

        t0 = ctx.read_pin('T0')

        temp_dir = ctx.temp_node_dir()
        for i in range(0, 3):
            if type(fixed[i]) == image.Image:
                medkit.write(fixed[i], os.path.join(temp_dir, 'f'+i+'.mhd'))
                fixed[i] = os.path.join(temp_dir, 'f'+i+'.mhd')

            if type(moving[i]) == image.Image:
                medkit.write(moving[i], os.path.join(temp_dir, 'm'+i+'.mhd'))
                moving[i] = os.path.join(temp_dir, 'm'+i+'.mhd')

            if (fixed[i] != None and type(fixed[i]) != str) or (moving[i] != None and type(moving[i]) != str):
                raise ValueError('Wrong values for input images, expected string or image')

        if fixed_mask == image.Image:
            medkit.write(fixed_mask, os.path.join(temp_dir, 'fmask.mhd'))
            fixed_mask = os.path.join(temp_dir, 'fmask.mhd')

        if moving_mask == image.Image:
            medkit.write(moving_mask, os.path.join(temp_dir, 'mmask.mhd'))
            moving_mask = os.path.join(temp_dir, 'mmask.mhd')

        if type(t0) == Parameters:
            t0.write(os.path.join(temp_dir, 't0.txt'))
            t0 = os.path.join(temp_dir, 't0.txt')

        param_file = os.path.abspath(self.param_file)
        run_elastix(param_file, fixed, moving, temp_dir, t0=t0, fixed_mask=fixed_mask, moving_mask=moving_mask, fp=fp, mp=mp)

        if self.is_pin_linked('Out'):
            ctx.write_pin('Out', medkit.read(os.path.join(temp_dir, 'result.0.nii')))

        if self.is_pin_linked('Transform'):
            ctx.write_pin('Transform', Parameters(os.path.join(temp_dir, 'TransformParameters.0.txt')))


flow.install_node_template(ElastixNode())

