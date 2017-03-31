from flow import FlowNode, FlowPin, FileProperty, install_node_template
import image
import medkit
import os
import subprocess
from elastix.parameters import Parameters

from pluto import pluto_class


transformix_exe = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'transformix.exe')
def run_transformix(param_file, out, img=None, df=False, jac=False):
    args = [transformix_exe, '-tp', param_file, '-out', out]
    if img:
        args.extend(['-in', img])
    if df:
        args.extend(['-def','all'])
    if jac:
        args.extend(['-jac','all'])
    proc = subprocess.Popen(args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    proc.wait()


@pluto_class
class TransformixNode(FlowNode):
    pins = [
        FlowPin('Image', FlowPin.In),
        FlowPin('Transform', FlowPin.In),
        FlowPin('Out', FlowPin.Out),
        FlowPin('Def', FlowPin.Out),
        FlowPin('Jac', FlowPin.Out),
    ]

    def __init__(self):
        super(TransformixNode, self).__init__()
        self.node_class = 'elastix.Transformix'
        self.title = 'Transformix'
        self.category = 'Elastix'
        self.value = ''

    def run(self, ctx):
        temp_dir = ctx.temp_node_dir()

        img = ctx.read_pin('Image')
        if img != None:
            if type(img) == image.Image:
                medkit.write(img, os.path.join(temp_dir, 'in.mhd'))
                img = os.path.join(temp_dir, 'in.mhd')

            if type(img) != str:
                raise ValueError('Unexpected type for pin \'Image\'')
  
        do_def = False
        do_jac = False
        if self.is_pin_linked('Def'):
            do_def = True
        if self.is_pin_linked('Jac'):
            do_jac = True

        transform = ctx.read_pin('Transform')
        if type(transform) == Parameters:
            transform.write(os.path.join(temp_dir, 'tp.txt'))
            transform = os.path.join(temp_dir, 'tp.txt')

        if type(transform) != str:
                raise ValueError('Unexpected type for pin \'Transform\'')

        run_transformix(transform, temp_dir, img=img, df=do_def, jac=do_jac)

        if self.is_pin_linked('Out') and img != None:
            ctx.write_pin('Out', medkit.read(os.path.join(temp_dir, 'result.nii')))

        if do_def:
            ctx.write_pin('Def', medkit.read(os.path.join(temp_dir, 'deformationField.nii')))
        if do_jac:
            ctx.write_pin('Jac', medkit.read(os.path.join(temp_dir, 'spatialJacobian.nii')))




install_node_template(TransformixNode())

