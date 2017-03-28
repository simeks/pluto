#include <Core/Common.h>

#include "Str.h"

namespace python
{
    Str::Str() : Object(PyUnicode_FromString(""))
    {
        
    }
    Str::Str(const char* str) : Object(PyUnicode_FromString(str))
    {
    }
    Str::Str(const python::Object& str) : Object(PyObject_Str(str.ptr()))
    {
    }

}
