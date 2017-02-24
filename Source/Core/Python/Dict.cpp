#include "Common.h"

#include "Object/Object.h"
#include "PythonCommon.h"
#include "Dict.h"

namespace python
{

    Dict::Dict() : Object(PyDict_New())
    {
    }
    Dict::Dict(const python::Object& d) : python::Object(d)
    {
    }

    bool Dict::has_key(const char* key) const
    {
        return PyMapping_HasKeyString(ptr(), key) != 0;
    }

    python::Object Dict::get(const char* key) const
    {
        return python::Object(PyDict_GetItemString(ptr(), key), true);
    }

    void Dict::set(const char* key, const python::Object& item)
    {
        PyDict_SetItemString(ptr(), key, item.ptr());
    }
    void Dict::clear()
    {
        PyDict_Clear(ptr());
    }
    int Dict::next(size_t* pos, PyObject** key, PyObject** value) const
    {
        return PyDict_Next(ptr(), (Py_ssize_t*)pos, key, value);
    }
    bool Dict::valid() const
    {
        return ptr() != nullptr;
    }
    Dict Dict::copy() const
    {
        if (!ptr())
            return Dict();

        Dict d(PyDict_Copy(ptr()));
        return d;
    }

}
