#include "Common.h"

#include "Image.h"


Image::Image() : 
    _ndims(0),
    _spacing(1, 1, 1),
    _pixel_type(image::PixelType_Unknown)
{
}
Image::Image(int ndims, const Vec3i& size, int pixel_type, const uint8_t* data) :
    _ndims(0),
    _spacing(1, 1, 1),
    _pixel_type(image::PixelType_Unknown)
{
    create(ndims, size, pixel_type, data);
}
Image::~Image()
{
}
void Image::create(int ndims, const Vec3i& size, int pixel_type, const uint8_t* data)
{
    assert(ndims <= 3);
    assert(pixel_type != image::PixelType_Unknown);

    int npy_type = numpy::pixel_type_to_numpy(pixel_type);

    int npy_ndims = 0;
    Py_intptr_t npy_dims[4] = { 1, 1, 1, 1 };
    numpy_shape(ndims, size, pixel_type, npy_ndims, npy_dims);

    _data = NumpyArray(npy_ndims, npy_dims, npy_type);

    _ndims = ndims;
    _size = size;
    _pixel_type = pixel_type;
    _step[0] = 0; _step[1] = 0; _step[2] = 0;
    for (int i = 0; i < _ndims; ++i)
        _step[i] = _data.stride(_ndims - 1 - i);

    if (data)
    {
        /// Assume memory allocated by numpy is contiguous
        size_t num_bytes = _size.x * _size.y * _size.z * pixel_size();
        memcpy(_data.data(), data, num_bytes);
    }
}
void Image::set(int ndims, const Vec3i& size, int pixel_type, const NumpyArray& data)
{
    _ndims = ndims;
    _size = size;
    _pixel_type = pixel_type;
    _data = data;
    _step[0] = 0; _step[1] = 0; _step[2] = 0;
    for (int i = 0; i < _ndims; ++i)
        _step[i] = _data.stride(_ndims - 1 - i);
}
void Image::release()
{
    _pixel_type = image::PixelType_Unknown;
    _data = NumpyArray();
    _step[0] = 0; _step[1] = 0; _step[2] = 0;
}

void Image::set_origin(const Vec3d& origin)
{
    _origin = origin;
}
void Image::set_spacing(const Vec3d& spacing)
{
    _spacing = spacing;
}

const Vec3i& Image::size() const
{
    return _size;
}
const Vec3d& Image::origin() const
{
    return _origin;
}
const Vec3d& Image::spacing() const
{
    return _spacing;
}
int Image::ndims() const
{
    return _ndims;
}

int Image::pixel_type() const
{
    return _pixel_type;
}
size_t Image::pixel_size() const
{
    return image::pixel_size(_pixel_type);
}
size_t Image::pixel_count() const
{
    return _size[0] * _size[1] * _size[2];
}

Image Image::clone() const
{
    Image img;
    img._ndims = _ndims;
    img._size = _size;
    img._data = _data.copy();
    assert(_data.is_contiguous());
    img._step[0] = 0; img._step[1] = 0; img._step[2] = 0;
    for (int i = 0; i < _ndims; ++i)
        img._step[i] = _data.stride(_ndims - 1 - i);
    img._pixel_type = _pixel_type;
    img._spacing = _spacing;
    img._origin = _origin;
    
    return img;
}

bool Image::valid() const
{
    return _data.valid();
}

const uint8_t* Image::ptr() const
{
    assert(valid());
    return (uint8_t*)_data.data();
}
uint8_t* Image::ptr()
{
    assert(valid());
    return (uint8_t*)_data.data();
}
void Image::copy_to(uint8_t* dest) const
{
    assert(valid());
    _data.copy_to(dest);
}
Image Image::convert_to(int type) const
{
    assert(valid());
    if (type == pixel_type())
    {
        return clone();
    }

    int ncomp = image::num_components(_pixel_type);
    int target_ncomp = image::num_components(type);

    assert_msg(ncomp == target_ncomp, "Conversion not supported"); // TODO:

    Image r = *this;
    r._data = _data.cast(numpy::pixel_type_to_numpy(type));
    r._pixel_type = type;
    r._step[0] = 0; r._step[1] = 0; r._step[2] = 0;
    for (int i = 0; i < _ndims; ++i)
        r._step[i] = _data.stride(_ndims - 1 - i);
    return r;
}


Image::Image(const Image& other) :
_data(other._data),
_ndims(other._ndims),
_size(other._size),
_origin(other._origin),
_spacing(other._spacing),
_pixel_type(other._pixel_type)
{
    _step[0] = 0; _step[1] = 0; _step[2] = 0;
    for (int i = 0; i < _ndims; ++i)
        _step[i] = _data.stride(_ndims - 1 - i);
}
Image& Image::operator=(const Image& other)
{
    _data = other._data;
    _ndims = other._ndims;
    _size = other._size;
    _origin = other._origin;
    _spacing = other._spacing;
    _pixel_type = other._pixel_type;

    _step[0] = 0; _step[1] = 0; _step[2] = 0;
    for (int i = 0; i < _ndims; ++i)
        _step[i] = _data.stride(_ndims - 1 - i);

    return *this;
}
const NumpyArray& Image::numpy_array() const
{
    return _data;
}
void Image::numpy_shape(int ndims, const Vec3i& size, int pixel_type, int& npy_ndims, npy_intp* npy_dims) const
{
    npy_ndims = ndims;

    int ncomp = image::num_components(pixel_type);

    for (int i = 0; i < ndims; ++i)
        npy_dims[i] = size[ndims-i-1];

    if (ncomp > 1)
    {
        npy_dims[ndims] = ncomp;
        ++npy_ndims;
    }
}
