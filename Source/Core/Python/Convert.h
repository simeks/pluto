#ifndef __CORE_PYTHON_CONVERT_H__
#define __CORE_PYTHON_CONVERT_H__


#include <Core/Image/Vec3.h>
#include <Core/Python/Class.h>
#include <Core/Python/Error.h>
#include <Core/Python/Object.h>
#include <Core/Python/Types.h>

/*
Some notes on from_python and to_python (Subject to changes):

from_python
===========
This conversion should not affect the reference count with one exception:
    PyObject* -> python::Object increases the ref count by one to account for the fact that 
    python::Object will decrease it when destroyed.

Pointers: Using from_python to convert to pointer types should _never_ return newly allocated objects 
    with copies to the original object stored in PyObject, as users should never have to be responsible
    of having to delete the returned object. Note: This may cause the pointer to become invalid if the PyObject
    gets deallocated.

    (TODO: Implement shared ptr)
    (Python classes created by using make_class in Class.h and specified as holding a shared pointer allows
    the user to convert the object to a shared pointer. The shared pointer works as a typical shared pointer.)

    Not supported:
        std::unique_ptr

References: Using from_python to convert to references leads to undefined behaviour and should be avoided for now.

Values: Converting to values will copy the source object onto a new object using the copy constructor.

to_python
=========
This converts an object to a PyObject and returns either a newly allocated PyObject or a new reference to an
    previous instance of a PyObject*, this depends on the value you want to convert. Important to remember is 
    that a new reference is returned, meaning the user has the responsibility to release the reference when done 
    with it. Using python::Object obj = python::to_python() does the reference handling for you.

Pointers: Behaviour of converting pointers to PyObject* depends on the type.
    For types defined using make_class in Class.h the following apply:
        Converting a pointer to an instance of a C++ class used in make_class will create a new PyObject
        which will hold a pointer referencing the same object as the pointer you want to convert. The PyObject takes
        no ownership of the object, meaning it will not attempt to delete the C++ object if it gets collected by GC itself.
        (TODO: To implement: Unless ownership transfer is explicitly specified)

        This of course forces the user to be careful with the lifetime of the object. Deleting an object in C++ to soon 
        will lead to the PyObject holding an invalid pointer, and expecting the PyObject to delete it will lead to a memory leak.

        Converting std::unique_ptr to python lets Python take ownership of an object, destryoing it when it gets collected by GC. 
        Suitable if you have some factory function in C++ function returning a new object and you want the caller to take ownership 
        of the object.

        (TODO: To implement: Shared ptr)
        (The prefered way to pass pointers between Python and C++ is to use shared pointers. Shared pointers works as expected,
        by passing a shared pointer to a PyObject the user is not required to manage the object.)

References: Undefined behaviour.

Values: Copies the value to a newly allocated object using the copy constructor.

*/

namespace python
{
    /// @remark This does not affect the reference count of given object.
    template<typename T>
    T from_python(PyObject* obj);

    template<typename T>
    T from_python(const Object& obj);

    /// Attempts to convert the given type to a python object and returns a new reference to said object.
    template<typename T>
    PyObject* to_python(const T& value);

    template<int N>
    PyObject* to_python(const char(&value)[N])
    {
        return PyUnicode_FromString(value);
    }

    template<typename T>
    PyObject* to_python(const std::vector<T>& value)
    {
        PyObject* l = PyList_New(value.size());
        for (size_t i = 0; i < value.size(); ++i)
            PyList_SetItem(l, i, to_python(value[i]));
        return l;
    }

    template<typename T, typename Converter>
    struct TypeConverter
    {
        TypeConverter();
    };
}

#include "Convert.inl"


#endif // __CORE_PYTHON_CONVERT_H__
