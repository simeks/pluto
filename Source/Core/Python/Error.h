#ifndef __PYTHON_ERROR_H__
#define __PYTHON_ERROR_H__

#include <Core/Python/PythonWrapper.h>

/*
Utilities for error handling between C++ and Python.

PYTHON_ERROR() : Sets a Python error and throws a C++ exception. The exception should ideally be 
                 handled right in connection to the call from Python (e.g. Function.cpp).
                 To make life easier the use of this macro should be limited as we do not want 
                 our C++ code to be usable only in connection with Python.
*/

#define PYTHON_ERROR(Exc, Text, ...) do{PyErr_Format(PyExc_##Exc, Text, __VA_ARGS__); throw python::ErrorSet();}while(0)

namespace python
{
    /// Exception, indicates that a python exception is set if thrown
    struct ErrorSet
    {
    };

}

#endif // __PYTHON_ERROR_H__
