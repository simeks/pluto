#include "Common.h"

#include "ImageObject.h"

#include "Python/PythonCommon.h"


PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(ImageObject, set_origin, Vec3d);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(ImageObject, set_spacing, Vec3d);

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(ImageObject, size);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(ImageObject, origin);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(ImageObject, spacing);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(ImageObject, ndims);

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(ImageObject, pixel_type);

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(ImageObject, array);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2(ImageObject, set_array, PyObject*, image::PixelType);

OBJECT_INIT_TYPE_FN(ImageObject)
{
    OBJECT_PYTHON_ADD_METHOD(ImageObject, ndims, "Returns the number of dimensions.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, size, "Returns the size of the image.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, pixel_type, "Returns the pixel type of the image.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, origin, "Returns the origin of the image.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, spacing, "Returns the spacing of the image.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, set_origin, "Sets the origin of the image.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, set_spacing, "Sets the spacing of the image.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, array, "Returns the image data array.");
    OBJECT_PYTHON_ADD_METHOD(ImageObject, set_array, "Sets the image data from a numpy array.");
}


IMPLEMENT_OBJECT(ImageObject, "Image", CORE_API);

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
        if (numpy::check_type(arr))
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
PyObject* ImageObject::array() const
{
    return (PyObject*)_image.numpy_array().object();
}
void ImageObject::set_array(PyObject* arr, image::PixelType pixel_type_hint)
{
    set_image(arr, pixel_type_hint);
}
