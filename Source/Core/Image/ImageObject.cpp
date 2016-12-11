#include "Common.h"

#include "ImageObject.h"

#include "Python/PythonCommon.h"


static PyObject* OBJECT_PYTHON_METHOD_NAME(ImageObject, ndims)(PyObject* self)
{
    ImageObject* object = object_cast<ImageObject>(py_object::object(self));
    if (object)
    {
        return PyLong_FromLong(object->ndims());
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* OBJECT_PYTHON_METHOD_NAME(ImageObject, size)(PyObject* self)
{
    ImageObject* object = object_cast<ImageObject>(py_object::object(self));
    if (object)
    {
        PyObject* size_tuple = PyTuple_New(3);
        for (int i = 0; i < 3; ++i)
            PyTuple_SetItem(size_tuple, i, PyLong_FromLong(object->size()[i]));

        return size_tuple;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject* OBJECT_PYTHON_METHOD_NAME(ImageObject, origin)(PyObject* self)
{
    ImageObject* object = object_cast<ImageObject>(py_object::object(self));
    if (object)
    {
        PyObject* tuple = PyTuple_New(3);
        for (int i = 0; i < 3; ++i)
            PyTuple_SetItem(tuple, i, PyFloat_FromDouble(object->origin()[i]));

        return tuple;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject* OBJECT_PYTHON_METHOD_NAME(ImageObject, spacing)(PyObject* self)
{
    ImageObject* object = object_cast<ImageObject>(py_object::object(self));
    if (object)
    {
        PyObject* tuple = PyTuple_New(3);
        for (int i = 0; i < 3; ++i)
            PyTuple_SetItem(tuple, i, PyFloat_FromDouble(object->spacing()[i]));

        return tuple;
    }
    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject* OBJECT_PYTHON_METHOD_NAME(ImageObject, set_origin)(PyObject* self, PyObject* args)
{
    ImageObject* object = object_cast<ImageObject>(py_object::object(self));
    if (object)
    {
        PyObject* origin_tuple = nullptr;
        if (PyArg_ParseTuple(args, "O:set_origin", &origin_tuple))
        {
            if (PyTuple_Check(origin_tuple))
            {
                Vec3d origin;
                int ndims = (int)PyTuple_Size(origin_tuple);
                for (int i = 0; i < ndims; ++i)
                {
                    PyObject* s = PyTuple_GetItem(origin_tuple, i);
                    if (PyFloat_Check(s))
                    {
                        origin[i] = PyFloat_AsDouble(s);
                    }
                    else if (PyLong_Check(s))
                    {
                        origin[i] = PyLong_AsLong(s);
                    }
                    else
                    {
                        PyErr_SetString(PyExc_ValueError, "Invalid value in tuple.");
                        return nullptr;
                    }
                }
                object->set_origin(origin);
            }
            else
            {
                PyErr_SetString(PyExc_TypeError, "Expected tuple.");
            }
        }
        else
        {
            return nullptr;
        }
    }

    Py_RETURN_NONE;
}
static PyObject* OBJECT_PYTHON_METHOD_NAME(ImageObject, set_spacing)(PyObject* self, PyObject* args)
{
    ImageObject* object = object_cast<ImageObject>(py_object::object(self));
    if (object)
    {
        PyObject* spacing_tuple = nullptr;
        if (PyArg_ParseTuple(args, "O:set_spacing", &spacing_tuple))
        {
            if (PyTuple_Check(spacing_tuple))
            {
                Vec3d spacing;
                int ndims = (int)PyTuple_Size(spacing_tuple);
                for (int i = 0; i < ndims; ++i)
                {
                    PyObject* s = PyTuple_GetItem(spacing_tuple, i);
                    if (PyFloat_Check(s))
                    {
                        spacing[i] = PyFloat_AsDouble(s);
                    }
                    else if (PyLong_Check(s))
                    {
                        spacing[i] = PyLong_AsLong(s);
                    }
                    else
                    {
                        PyErr_SetString(PyExc_ValueError, "Invalid value in tuple.");
                        return nullptr;
                    }
                }
                object->set_spacing(spacing);
            }
            else
            {
                PyErr_SetString(PyExc_TypeError, "Expected tuple.");
            }
        }
        else
        {
            return nullptr;
        }
    }

    Py_RETURN_NONE;
}
static PyObject* OBJECT_PYTHON_METHOD_NAME(ImageObject, pixel_type)(PyObject* self)
{
    ImageObject* object = object_cast<ImageObject>(py_object::object(self));
    if (object)
    {
        PyObject* str = nullptr;
        str = PyUnicode_FromString(image::pixel_type_to_string(object->pixel_type()));

        if (str != nullptr)
            return str;
    }
    Py_RETURN_NONE;
}
static PyObject* OBJECT_PYTHON_METHOD_NAME(ImageObject, array)(PyObject* self)
{
    ImageObject* object = object_cast<ImageObject>(py_object::object(self));
    if (object)
    {
        PyObject* npy_arr = (PyObject*)object->image().numpy_array().object();
        if (npy_arr)
        {
            Py_INCREF(npy_arr);
            return npy_arr;
        }
    }
    Py_RETURN_NONE;
}
static PyObject* OBJECT_PYTHON_METHOD_NAME(ImageObject, set_array)(PyObject* self, PyObject* args)
{
    ImageObject* object = object_cast<ImageObject>(py_object::object(self));
    if (object)
    {
        PyObject* arr = nullptr;
        PyObject* hint_str = nullptr;
        if (PyArg_ParseTuple(args, "OU:set_array", &arr, &hint_str))
        {
            int hint = image::PixelType_Unknown;
            if (hint_str)
            {
                hint = image::string_to_pixel_type(PyUnicode_AsUTF8(hint_str));
            }

            if (!object->set_image(arr, hint))
            {
                return nullptr;
            }
        }
        else
        {
            return nullptr;
        }
    }
    Py_RETURN_NONE;
}

OBJECT_INIT_TYPE_FN(ImageObject)
{
    OBJECT_PYTHON_ADD_METHOD(ImageObject, ndims, METH_NOARGS, "Returns the number of dimensions.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, size, METH_NOARGS, "Returns the size of the image.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, pixel_type, METH_NOARGS, "Returns the pixel type of the image.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, origin, METH_NOARGS, "Returns the origin of the image.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, spacing, METH_NOARGS, "Returns the spacing of the image.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, set_origin, METH_VARARGS, "Sets the origin of the image.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, set_spacing, METH_VARARGS, "Sets the spacing of the image.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, array, METH_NOARGS, "Returns the image data array.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, set_array, METH_VARARGS, "Sets the image data from a numpy array.");
}


IMPLEMENT_OBJECT(ImageObject, "Image");

ImageObject::ImageObject()
{
}
ImageObject::ImageObject(const Image& img) :
    _image(img)
{
}
ImageObject::~ImageObject()
{
}
void ImageObject::set_image(const Image& img)
{
    _image = img;
}
void ImageObject::set_origin(const Vec3d& origin)
{
    _image.set_origin(origin);
}
void ImageObject::set_spacing(const Vec3d& spacing)
{
    _image.set_spacing(spacing);
}

const Vec3i& ImageObject::size() const
{
    assert(_image.valid());
    return _image.size();
}
const Vec3d& ImageObject::origin() const
{
    return _image.origin();
}
const Vec3d& ImageObject::spacing() const
{
    return _image.spacing();
}
int ImageObject::ndims() const
{
    assert(_image.valid());
    return _image.ndims();
}
int ImageObject::pixel_type() const
{
    return _image.pixel_type();
}
Image& ImageObject::image()
{
    return _image;
}
const Image& ImageObject::image() const
{
    return _image;
}

ImageObject::ImageObject(const ImageObject& other)
{
    _image = other._image;
}
ImageObject& ImageObject::operator=(const ImageObject& other)
{
    _image = other._image;
    return *this;
}
int ImageObject::object_init(const Tuple& args, const Dict&)
{
    PyObject* arr = nullptr;
    PyObject* hint_str = nullptr;
    if (PyArg_ParseTuple(args.tuple(), "|OU:__init__", &arr, &hint_str))
    {
        if (arr)
        {
            int hint = image::PixelType_Unknown;
            if (hint_str)
            {
                hint = image::string_to_pixel_type(PyUnicode_AsUTF8(hint_str));
            }
            else
            {
                PyErr_SetString(PyExc_TypeError, "Pixel type expected.");
                return -1;
            }

            if (!set_image(arr, hint))
            {
                return -1;
            }
        }
    }
    else
    {
        return -1;
    }
    return 0;
}
bool ImageObject::set_image(PyObject* npy_array, int pixel_type_hint)
{
    NumpyArray arr((PyArrayObject*)npy_array);

    int ndims = arr.ndims();

    int nc = image::num_components(pixel_type_hint);
    if (nc > 1)
    {
        --ndims;
        if (arr.dims()[ndims] != nc)
        {
            PyErr_SetString(PyExc_ValueError, "Mismatch in number of components per element between array and hinted element type.");
            return false;
        }
    }

    // Perform casting if neeed
    if (arr.type() != numpy::pixel_type_to_numpy(pixel_type_hint))
    {
        arr = arr.cast(numpy::pixel_type_to_numpy(pixel_type_hint));
    }

    Vec3i size(1, 1, 1);
    for (int i = 0; i < ndims; ++i)
        size[i] = int(arr.dims()[ndims - i - 1]);

    Py_INCREF(npy_array);
    _image.set(ndims, size, pixel_type_hint, arr);

    return true;
}