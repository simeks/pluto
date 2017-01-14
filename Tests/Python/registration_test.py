import image
import imageutils
import registration

settings = {
    'step_size': 0.5,
    'regularization_weight': 0.05,
    'block_size': (12, 12, 12),
    'pyramid_level_min': 0,
    'pyramid_level_max': 6,
}

e = registration.RegistrationEngine('blocked_graph_cut', image.PixelType_Float32, settings)
i = imread('chess.png')
ir = imageutils.split_channels(i)[0]

i2 = imread('chess2.png')
i2r = imageutils.split_channels(i2)[0]

df = e.execute(ir, i2r)

out = registration.transform(i, df)
imwrite(out, 'out.png')
