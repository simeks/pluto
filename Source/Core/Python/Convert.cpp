#include "Common.h"
#include "Convert.h"
#include "Dict.h"
#include "Sequence.h"
#include "Tuple.h"

#include "Image/Types.h"
#include "Object/Object.h"

#define INT_FROM_PYTHON(T) \
    template<> \
    CORE_API T from_python(PyObject* obj) \
    { \
        return (T)PyLong_AsLongLong(obj); \
    }

#define UINT_FROM_PYTHON(T) \
    template<> \
    CORE_API T from_python(PyObject* obj) \
    { \
        if (PyLong_Check(obj)) \
        { \
            return (T)PyLong_AsUnsignedLongLong(obj); \
        } \
        else \
        { \
            long long l = PyLong_AsLongLong(obj); \
            if (l < 0) \
                PyErr_SetString(PyExc_OverflowError, "Cannot convert negative value to unsigned"); \
            return (T)l; \
        } \
    } 

#define FLOAT_FROM_PYTHON(T) \
    template<> \
    CORE_API T from_python(PyObject* obj) \
    { \
        return (T)PyFloat_AsDouble(obj); \
    }

#define VEC3_FROM_PYTHON(T) \
    template<> \
    CORE_API Vec3<T> from_python(PyObject* obj) \
    { \
        if (PySequence_Check(obj) && PySequence_Size(obj)) \
        { \
            Vec3<T> v; \
            for (int i = 0; i < std::min<Py_ssize_t>(PySequence_Size(obj), 3); ++i) \
            { \
                v[i] = from_python<T>(PySequence_GetItem(obj, i)); \
            } \
            return v; \
        } \
        PyErr_SetString(PyExc_ValueError, "Failed to convert value to Vec3i"); \
        return Vec3<T>(); \
    }


#define INT_TO_PYTHON(T) \
    template<> \
    CORE_API PyObject* to_python(const T& value) \
    { \
        return PyLong_FromLongLong((long long)value); \
    }
#define UINT_TO_PYTHON(T) \
    template<> \
    CORE_API PyObject* to_python(const T& value) \
    { \
        return PyLong_FromUnsignedLongLong((unsigned long long)value); \
    }
#define FLOAT_TO_PYTHON(T) \
    template<> \
    CORE_API PyObject* to_python(const T& value) \
    { \
        return PyFloat_FromDouble((double)value); \
    }
#define VEC3_TO_PYTHON(T) \
    template<> \
    CORE_API PyObject* to_python(const Vec3<T>& value) \
    { \
        PyObject* t = PyTuple_New(3); \
        PyTuple_SetItem(t, 0, to_python<T>(value.x)); \
        PyTuple_SetItem(t, 1, to_python<T>(value.y)); \
        PyTuple_SetItem(t, 2, to_python<T>(value.z)); \
        return t; \
    }

namespace python
{
    INT_FROM_PYTHON(int8_t);
    INT_FROM_PYTHON(int16_t);
    INT_FROM_PYTHON(int32_t);
    INT_FROM_PYTHON(int64_t);

    UINT_FROM_PYTHON(uint8_t);
    UINT_FROM_PYTHON(uint16_t);
    UINT_FROM_PYTHON(uint32_t);
    UINT_FROM_PYTHON(uint64_t);

    FLOAT_FROM_PYTHON(float);
    FLOAT_FROM_PYTHON(double);

    VEC3_FROM_PYTHON(uint8_t);
    VEC3_FROM_PYTHON(int);
    VEC3_FROM_PYTHON(float);
    VEC3_FROM_PYTHON(double);

    template<>
    CORE_API const char* from_python(PyObject* obj)
    {
        if (PyUnicode_Check(obj))
            return PyUnicode_AsUTF8(obj);
        PyErr_SetString(PyExc_ValueError, "Expected string");
        return nullptr;
    }

    template<>
    CORE_API std::string from_python(PyObject* obj)
    {
        if (PyUnicode_Check(obj))
            return PyUnicode_AsUTF8(obj);
        PyErr_SetString(PyExc_ValueError, "Expected string");
        return "";
    }

    template<>
    CORE_API Tuple from_python(PyObject* obj)
    {
        if (PySequence_Check(obj) || PyTuple_Check(obj))
        {
            return Tuple(obj);
        }
        Tuple t(1);
        t.set(0, obj);
        return t;
    }

    template<>
    CORE_API Dict from_python(PyObject* obj)
    {
        if (PyDict_Check(obj))
            return Dict(obj);
        PyErr_SetString(PyExc_ValueError, "Expected dict");
        return Dict();
    }

    template<>
    CORE_API void from_python(PyObject* )
    {
    }

    template<>
    CORE_API bool from_python(PyObject* obj)
    {
        if (PyBool_Check(obj))
            return (obj == Py_True);
        PyErr_SetString(PyExc_ValueError, "Expected bool");
        return false;
    }


    INT_TO_PYTHON(int8_t);
    INT_TO_PYTHON(int16_t);
    INT_TO_PYTHON(int32_t);
    INT_TO_PYTHON(int64_t);

    UINT_TO_PYTHON(uint8_t);
    UINT_TO_PYTHON(uint16_t);
    UINT_TO_PYTHON(uint32_t);
    UINT_TO_PYTHON(uint64_t);

    FLOAT_TO_PYTHON(float);
    FLOAT_TO_PYTHON(double);

    VEC3_TO_PYTHON(uint8_t);
    VEC3_TO_PYTHON(int);
    VEC3_TO_PYTHON(float);
    VEC3_TO_PYTHON(double);

    template<>
    CORE_API PyObject* to_python(const char* const& value)
    {
        return PyUnicode_FromString(value);
    }
    template<>
    CORE_API PyObject* to_python(const std::string& value)
    {
        return PyUnicode_FromString(value.c_str());
    }

    template<>
    CORE_API PyObject* to_python(const Tuple& value)
    {
        PyObject* o = value.tuple();
        Py_XINCREF(o); // Increase ref as the Tuple destructor will decrease it
        return o;
    }

    template<>
    CORE_API PyObject* to_python(const Dict& value)
    {
        PyObject* o = value.dict();
        Py_XINCREF(o); // Increase ref as the Tuple destructor will decrease it
        return o;
    }

    template<>
    CORE_API PyObject* to_python(const Guid& value)
    {
        return PyUnicode_FromString(guid::to_string(value).c_str());
    }
    template<>
    CORE_API PyObject* to_python(const bool& value)
    {
        if (value)
        {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }


}
