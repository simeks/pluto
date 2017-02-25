#ifndef __PYTHON_OBJECT_H__
#define __PYTHON_OBJECT_H__

#include "PythonWrapper.h"

class PythonClass;

namespace python
{
    class Dict;
    class Tuple;

    struct _Borrowed {};
    typedef _Borrowed* Borrowed; /// Type trick to specifies that a PyObject* is borrowed reference

    class CORE_API Object
    {
    public:
        Object();
        Object(PyObject* obj);
        /// Constructor for borrowed references, this will increase the ref count of obj
        Object(Borrowed obj);
        ~Object();

        Object(const Object& other);
        Object& operator=(const Object& other);
        
        /// Returns a borrowed reference
        PyObject* ptr() const;

        bool is_none() const;
        bool is_instance(const Object& type) const;
        bool is_instance(PyTypeObject* type) const;

    private:
        PyObject* _obj;
    };

    CORE_API Object None();
    CORE_API Object True();
    CORE_API Object False();

    PyObject* incref(PyObject* obj);
    PyObject* xincref(PyObject* obj);

    PyObject* decref(PyObject* obj);
    PyObject* xdecref(PyObject* obj);

    CORE_API bool hasattr(const Object& obj, const char* key);

    CORE_API Object getattr(const Object& obj, const char* key);
    CORE_API Object getattr(const Object& obj, const char* key, const Object& default);
    CORE_API void setattr(const Object& obj, const char* key, const Object& value);

    CORE_API Object call(const Object& obj, const Tuple& args, const Dict& kw);

    /// @brief Adds the specified class to the given object
    CORE_API void def(const Object& m, const char* name, PythonClass* cls);

    /// @brief Adds the specified object to the given object
    template<typename T>
    void def(const Object& m, const char* name, const T& obj);

} // namespace python

#include "Object.inl"

#endif // __PYTHON_OBJECT_H__