#ifndef __PYTHON_OBJECT_H__
#define __PYTHON_OBJECT_H__

#include "PythonCommon.h"

namespace python
{
    class CORE_API Object
    {
    public:
        Object();
        /// Constructor increases the reference count of obj
        Object(PyObject* obj);
        ~Object();

        Object(const Object& other);
        Object& operator=(const Object& other);

        PyObject* ptr() const;

    private:
        PyObject* _obj;
    };

    PyObject* incref(PyObject* obj);
    PyObject* xincref(PyObject* obj);

    PyObject* decref(PyObject* obj);
    PyObject* xdecref(PyObject* obj);

    template<typename T>
    T incref(T obj);
    template<typename T>
    T xincref(T obj);

    template<typename T>
    T decref(T obj);
    template<typename T>
    T xdecref(T obj);

    CORE_API bool hasattr(const Object& obj, const char* key);

    CORE_API Object getattr(const Object& obj, const char* key);
    CORE_API Object getattr(const Object& obj, const char* key, const Object& default);

    CORE_API void setattr(const Object& obj, const char* key, const Object& value);

} // namespace python

#include "Object.inl"

#endif // __PYTHON_OBJECT_H__