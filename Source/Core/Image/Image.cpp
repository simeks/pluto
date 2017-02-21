#include "Common.h"

#include "Image.h"
#include "Python/Module.h"
#include "Python/NumPy.h"
#include "Python/Object.h"
#include "Python/PythonCommon.h"
#include "Types.h"

namespace
{
    PyObject* _image_py_type = nullptr;
    PyObject* image_py_type()
    {
        if (!_image_py_type)
        {
            python::Object img_mod = python::import("image");
            if (!img_mod.ptr())
                return nullptr;

            python::Object image = python::getattr(img_mod, "Image");
            if (!PyType_Check(image.ptr()))
                return nullptr;

            _image_py_type = image.ptr();
            Py_INCREF(_image_py_type);
        }
        return _image_py_type;
    }
}

namespace python
{
    template<>
    CORE_API Image from_python(PyObject* obj)
    {
        if (numpy::check_type(obj))
        {
            image::PixelType pixel_type = python::getattr(obj, "pixel_type", image::PixelType_Unknown);
            
            Image img(from_python<NumpyArray>(obj), pixel_type);

            if (PyObject_HasAttrString(obj, "origin"))
                img.set_origin(python::getattr(obj, "origin", Vec3d(0, 0, 0)));
            if (PyObject_HasAttrString(obj, "spacing"))
                img.set_origin(python::getattr(obj, "spacing", Vec3d(1, 1, 1)));
            
            return img;
        }
        
        PYTHON_ERROR_R(ValueError, Image(), "Failed to convert object of type '%s' to Image", obj->ob_type->tp_name);
    }

    template<>
    CORE_API PyObject* to_python(const Image& value)
    {
        if (!value.valid())
            Py_RETURN_NONE;

        PyObject* args = PyTuple_New(2);
        PyTuple_SetItem(args, 0, to_python(value.data()));
        PyTuple_SetItem(args, 1, to_python((image::PixelType)value.pixel_type()));

        PyObject* imgobj = PyObject_Call(image_py_type(), args, nullptr);
        Py_DECREF(args);

        PyObject_SetAttrString(imgobj, "origin", to_python(value.origin()));
        PyObject_SetAttrString(imgobj, "spacing", to_python(value.spacing()));

        return imgobj;
    }
}

Image::Image() : 
    _ndims(0),
    _spacing(1, 1, 1),
    _pixel_type(image::PixelType_Unknown)
{
}
Image::Image(int ndims, const Vec3i& size, int pixel_type, const uint8_t* data) :
    _ndims(0),
    _pixel_type(image::PixelType_Unknown),
    _origin(0, 0, 0),
    _spacing(1, 1, 1)
{
    create(ndims, size, pixel_type, data);
}
Image::Image(const NumpyArray& data, int pixel_type) :
    _data(data),
    _pixel_type(pixel_type),
    _origin(0, 0, 0),
    _spacing(1, 1, 1)
{
    _ndims = _data.ndims();
    if (image::num_components(pixel_type) > 1)
        --_ndims;

    _size = Vec3i(1, 1, 1);
    for (int i = 0; i < _ndims; ++i)
        _size[i] = (int)_data.shape()[_ndims - i - 1];
    _pixel_type = pixel_type;
    _data = data;
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

    if (data)
    {
        /// Assume memory allocated by numpy is contiguous
        size_t num_bytes = _size.x * _size.y * _size.z * pixel_size();
        memcpy(_data.data(), data, num_bytes);
    }
}
void Image::release()
{
    _pixel_type = image::PixelType_Unknown;
    _data = NumpyArray();
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
    img._pixel_type = _pixel_type;
    img._spacing = _spacing;
    img._origin = _origin;
    
    return img;
}
Image Image::reshape(int ndims, const Vec3i& size) const
{
    int npy_ndims = 0;
    Py_intptr_t npy_dims[4] = { 1, 1, 1, 1 };
    numpy_shape(ndims, size, _pixel_type, npy_ndims, npy_dims);

    Image r = *this;
    r._data = _data.reshape(npy_ndims, npy_dims);
    r._ndims = ndims;
    r._size = size;
    return r;
}

bool Image::valid() const
{
    return _data.valid();
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
}
Image& Image::operator=(const Image& other)
{
    _data = other._data;
    _ndims = other._ndims;
    _size = other._size;
    _origin = other._origin;
    _spacing = other._spacing;
    _pixel_type = other._pixel_type;

    return *this;
}
const NumpyArray& Image::data() const
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
