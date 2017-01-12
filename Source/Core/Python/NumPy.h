#ifndef __CORE_PYTHON_NUMPY_H__
#define __CORE_PYTHON_NUMPY_H__

#include "PythonWrapper.h"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <numpy/npy_common.h>
#include <numpy/ndarraytypes.h>

namespace numpy
{
    CORE_API void initialize();

    CORE_API int pixel_type_to_numpy(int pixel_type);

    CORE_API bool check_type(PyObject* obj);
}

class CORE_API NumpyArray
{
public:
    NumpyArray();
    explicit NumpyArray(int ndims, Py_intptr_t* dims, int type);
    explicit NumpyArray(PyArrayObject* arr);
    ~NumpyArray();

    void* data() const;

    int ndims() const;
    Py_intptr_t* dims() const;
    Py_intptr_t stride(int i) const;
    Py_intptr_t* strides() const;

    /// Returns the total number of bytes consumed by the array
    Py_intptr_t nbytes() const;

    /// Returns the total number of elements in the array
    Py_intptr_t nelems() const;

    /// Returns the size per element
    Py_intptr_t element_size() const;

    int type() const;

    bool valid() const;

    bool is_contiguous() const;

    NumpyArray copy() const;

    /// Copy to C-style array
    void copy_to(uint8_t* dest) const;
    /// Copy from C-style array
    void copy_from(uint8_t* src) const;

    NumpyArray cast(int type) const;
    NumpyArray cast(PyArray_Descr* desc) const;

    NumpyArray(const NumpyArray& other);
    NumpyArray& operator=(const NumpyArray& other);

    PyArrayObject* object() const;

private:
    PyArrayObject* _arr;

};

#endif // __CORE_PYTHON_NUMPY_H__
