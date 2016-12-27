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
    case image::PixelType_UInt16:
        return NPY_UINT16;
    case image::PixelType_UInt32:
        return NPY_UINT32;
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

NumpyArray::NumpyArray() : _arr(nullptr)
{
}
NumpyArray::NumpyArray(int ndims, Py_intptr_t* dims, int type)
{
    _arr = (PyArrayObject*)PyArray_SimpleNew(ndims, dims, type);
}
NumpyArray::NumpyArray(PyArrayObject* arr) : _arr(arr)
{
}
NumpyArray::~NumpyArray()
{
    Py_XDECREF(_arr);
}

void* NumpyArray::data() const
{
    return PyArray_DATA(_arr);
}
int NumpyArray::ndims() const
{
    return PyArray_NDIM(_arr);
}
Py_intptr_t* NumpyArray::dims() const
{
    return PyArray_DIMS(_arr);
}
Py_intptr_t NumpyArray::stride(int i) const
{
    return PyArray_STRIDE(_arr, i);
}
Py_intptr_t* NumpyArray::strides() const
{
    return PyArray_STRIDES(_arr);
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
    return NumpyArray((PyArrayObject*)PyArray_NewCopy(_arr, NPY_CORDER));
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
    return NumpyArray((PyArrayObject*)PyArray_Cast(_arr, type));
}
NumpyArray NumpyArray::cast(PyArray_Descr* desc) const
{
    return NumpyArray((PyArrayObject*)PyArray_CastToType(_arr, desc, 0));
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

PyArrayObject* NumpyArray::object() const
{
    return _arr;
}
