import flow
import image
import medkit
import os
import subprocess
from elastix.parameters import Parameters

from pluto import pluto_class


elastix_exe = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'elastix.exe')
def run_elastix(param_file, fixed_imgs, moving_imgs, out, fixed_mask=None, moving_mask=None, t0=None):
    args = [elastix_exe, '-p', param_file, '-out', out]
    for i, f in enumerate(fixed_imgs):
        args.extend(['-f'+str(i), f])
    for i, m in enumerate(moving_imgs):
        args.extend(['-m'+str(i), m])

    if fixed_mask != None:
        args.extend(['-fMask', fixed_mask])
    if moving_mask != None:
        args.extend(['-mMask', moving_mask])

    if t0 != None:
        args.extend(['-t0', t0])

    proc = subprocess.Popen(args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    proc.wait()


@pluto_class
class ElastixNode(flow.Node):
    pins = [
        flow.Pin('F0', flow.Pin.In),
        flow.Pin('M0', flow.Pin.In),
        flow.Pin('T0', flow.Pin.In),
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
        f0 = ctx.read_pin('F0')
        m0 = ctx.read_pin('M0')
        t0 = ctx.read_pin('T0')

        temp_dir = ctx.temp_node_dir()
        if type(f0) == image.Image:
            medkit.write(f0, os.path.join(temp_dir, 'f0.mhd'))
            f0 = os.path.join(temp_dir, 'f0.mhd')

        if type(m0) == image.Image:
            medkit.write(m0, os.path.join(temp_dir, 'm0.mhd'))
            m0 = os.path.join(temp_dir, 'm0.mhd')

        if type(f0) != str or type(m0) != str:
            return # TODO: Error

        if type(t0) == Parameters:
            t0.write(os.path.join(temp_dir, 't0.txt'))
            t0 = os.path.join(temp_dir, 't0.txt')


        param_file = os.path.abspath(self.param_file)
        run_elastix(param_file, [f0], [m0], temp_dir, t0=t0)

        if self.is_pin_linked('Out'):
            ctx.write_pin('Out', medkit.read(os.path.join(temp_dir, 'result.0.nii')))

        if self.is_pin_linked('Transform'):
            ctx.write_pin('Transform', Parameters(os.path.join(temp_dir, 'TransformParameters.0.txt')))


flow.install_node_template(ElastixNode())

