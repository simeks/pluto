#include "Common.h"

#include "Image/Types.h"
#include "NumPy.h"
#include "PythonCommon.h"

#include <numpy/arrayobject.h>

void numpy::initialize()
{
    if (_import_array() < 0)
    {
        PyErr_Print(); PyErr_SetString(PyExc_ImportError, "numpy.core.multiarray failed to import");
    }
}
int numpy::pixel_type_to_numpy(int pixel_type)
{
    switch (pixel_type)
    {
    case image::PixelType_UInt8:
    case image::PixelType_Vec3u8:
    case image::PixelType_Vec4u8:
        return NPY_UINT8;
    case image::PixelType_Int8:
        return NPY_INT8;
    case image::PixelType_UInt16:
        return NPY_UINT16;
    case image::PixelType_Int16:
        return NPY_INT16;
    case image::PixelType_UInt32:
        return NPY_UINT32;
    case image::PixelType_Int32:
        return NPY_INT32;
    case image::PixelType_Float32:
    case image::PixelType_Vec3f:
    case image::PixelType_Vec4f:
        return NPY_FLOAT32;
    case image::PixelType_Float64:
    case image::PixelType_Vec3d:
    case image::PixelType_Vec4d:
        return NPY_FLOAT64;
    }
    return -1;
}
bool numpy::check_type(PyObject* obj)
{
    return obj && PyArray_Check(obj) != 0;
}

namespace python_convert
{
    template<>
    CORE_API NumpyArray from_python(PyObject* obj)
    {
        if (PyArray_Check(obj))
        {
            return NumpyArray((PyArrayObject*)obj);
        }

        PYTHON_ERROR_R(ValueError, NumpyArray(), "Failed to convert object of type '%s' to NumpyArray", obj->ob_type->tp_name);
    }

    template<>
    CORE_API PyObject* to_python(const NumpyArray& value)
    {
        PyObject* obj = value.object();
        if (!obj)
            Py_RETURN_NONE;

        Py_INCREF(obj);
        return obj;
    }
}

NumpyArray::NumpyArray() : _arr(nullptr)
{
}
NumpyArray::NumpyArray(int ndims, Py_intptr_t* dims, int type)
{
    _arr = (PyArrayObject*)PyArray_SimpleNew(ndims, dims, type);
}
NumpyArray::NumpyArray(PyArrayObject* arr) : _arr(arr)
{
    Py_INCREF(_arr);
}
NumpyArray::~NumpyArray()
{
    Py_XDECREF(_arr);
}

Py_intptr_t NumpyArray::nbytes() const
{
    return PyArray_NBYTES(_arr);
}
Py_intptr_t NumpyArray::nelems() const
{
    return PyArray_SIZE(_arr);
}
Py_intptr_t NumpyArray::element_size() const
{
    return PyArray_ITEMSIZE(_arr);
}
int NumpyArray::type() const
{
    return PyArray_TYPE(_arr);
}
bool NumpyArray::valid() const
{
    return _arr != nullptr;
}
bool NumpyArray::is_contiguous() const
{
    return PyArray_ISCONTIGUOUS(_arr) != 0;
}
NumpyArray NumpyArray::copy() const
{
    PyObject* obj = PyArray_NewCopy(_arr, NPY_CORDER);
    NumpyArray arr((PyArrayObject*)obj);
    Py_DECREF(obj);

    return arr;
}
void NumpyArray::copy_to(uint8_t* dest) const
{
    if (!is_contiguous())
    {
        assert_msg(false, "Not implemented"); // TODO:
    }
    else
    {
        memcpy(dest, data(), nbytes());
    }
}
void NumpyArray::copy_from(uint8_t* src) const
{
    if (!is_contiguous())
    {
        assert_msg(false, "Not implemented"); // TODO:
    }
    else
    {
        memcpy(data(), src, nbytes());
    }
}
NumpyArray NumpyArray::cast(int type) const
{
    PyObject* obj = PyArray_Cast(_arr, type);
    NumpyArray arr((PyArrayObject*)obj);
    Py_DECREF(obj);

    return arr;
}
NumpyArray NumpyArray::cast(PyArray_Descr* desc) const
{
    PyObject* obj = PyArray_CastToType(_arr, desc, 0);
    NumpyArray arr((PyArrayObject*)obj);
    Py_DECREF(obj);

    return arr;
}
NumpyArray NumpyArray::contiguous() const
{
    PyArrayObject* obj = PyArray_GETCONTIGUOUS(_arr);
    NumpyArray arr(obj);
    Py_DECREF(obj);

    return arr;
}
NumpyArray::NumpyArray(const NumpyArray& other)
{
    _arr = other._arr;
    Py_XINCREF(_arr);
}
NumpyArray& NumpyArray::operator=(const NumpyArray& other)
{
    _arr = other._arr;
    Py_XINCREF(_arr);
    return *this;
}

bool numpy::check(PyObject* obj)
{
    return PyArray_Check(obj);
}
