from flow import FlowNode, ArrayFlowPin, FlowPin, FileProperty, install_node_template
import image
from pluto import pluto_class

from registration import RegistrationEngine

def parse_settings_old(file):
    settings = {}
    with open(file, 'r') as f:
        for l in f:
            t = l.split('=')
            if len(t) == 2:
                key = t[0].lower()
                value = t[1].strip()
                if key == 'pyramid_levels':
                    settings['pyramid_level_max'] = int(value)
                elif key == 'max_resolution':
                    settings['pyramid_level_min'] = int(value)
                elif key == 'stepsize':
                    settings['step_size'] = float(value)
                elif key == 'regularization_weight':
                    settings['regularization_weight'] = float(value)
                elif key == 'regularization_weights':
                    settings['regularization_weights'] = [float(v) for v in value.split(' ')]
                elif key == 'normalize_images':
                    settings['normalize_images'] = bool(int(value))
                elif key == 'block_size':
                    settings['block_size'] = [int(v) for v in value.split(' ')]
                else:
                    settings[key] = value
    return settings

@pluto_class
class RegistrationNode(FlowNode):
    pins = [
        ArrayFlowPin('Fixed', FlowPin.In),
        ArrayFlowPin('Moving', FlowPin.In),

        FlowPin('ConstraintMask', FlowPin.In),
        FlowPin('ConstraintValues', FlowPin.In),

        FlowPin('StartingGuess', FlowPin.In),

        FlowPin('Deformation', FlowPin.Out),
    ]
    properties = [
        FileProperty('param_file', '')
    ]

    def __init__(self):
        super(RegistrationNode, self).__init__()
        self.node_class = 'registration.Registration' 
        self.title = 'Registration'
        self.category = 'Registration'

    def run(self, ctx):
        settings = {
            'step_size': 0.5,
            'regularization_weight': 0.05,
            'block_size': (12, 12, 12),
            'pyramid_level_min': 0,
            'pyramid_level_max': 6,
        }

        if self.param_file is not '':
            settings.update(parse_settings_old(self.param_file))

        eng = RegistrationEngine('blocked_graph_cut', image.PixelType_Float64, settings)

        if self.is_pin_linked('ConstraintValues') or self.is_pin_linked('ConstraintMask'):
            eng.set_contraints(ctx.read_pin('ConstraintValues'), ctx.read_pin('ConstraintMask'))

        if self.is_pin_linked('StartingGuess'):
            eng.set_starting_guess(ctx.read_pin('StartingGuess'))

        fixed = ctx.read_pin('Fixed')
        moving = ctx.read_pin('Moving')

        df = eng.execute(fixed, moving)
        ctx.write_pin('Deformation', df)

install_node_template(RegistrationNode())