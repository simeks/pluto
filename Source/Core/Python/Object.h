#ifndef __PYTHON_OBJECT_H__
#define __PYTHON_OBJECT_H__

#include "PythonWrapper.h"

#include <Core/Object/PythonClass.h>

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

    CORE_API bool hasattr(const Object& obj, const char* key);

    CORE_API Object getattr(const Object& obj, const char* key);
    CORE_API Object getattr(const Object& obj, const char* key, const Object& default);
    CORE_API void setattr(const Object& obj, const char* key, const Object& value);

    /// @brief Adds the specified class to the given module
    void def(const Object& m, const char* name, PythonClass* cls);

    /// @brief Adds the specified object to the given module
    template<typename T>
    void def(const Object& m, const char* name, const T& obj);

} // namespace python

#include "Object.inl"

#endif // __PYTHON_OBJECT_H__