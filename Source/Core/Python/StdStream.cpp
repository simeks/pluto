#include "Common.h"

#include "StdStream.h"

#include "PythonType.h"
#include "PythonCommon.h"


PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(PyStdStream, write, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0(PyStdStream, flush);

OBJECT_INIT_TYPE_FN(PyStdStream)
{
    OBJECT_PYTHON_ADD_METHOD(PyStdStream, write, METH_VARARGS, "");
    OBJECT_PYTHON_ADD_METHOD(PyStdStream, flush, METH_NOARGS, "");
}

IMPLEMENT_OBJECT(PyStdStream, "StdStream");

PyStdStream::PyStdStream()
    : _fn(nullptr),
    _data(nullptr)
{
}
PyStdStream::~PyStdStream()
{
}

void PyStdStream::write(const char* text)
{
    if (text) 
    {
        if (_fn)
        {
            _fn(_data, text);
        }
    }
}
PyObject* PyStdStream::flush()
{
    Py_RETURN_NONE;
}

void PyStdStream::set_callback(Callback* fn, void* data)
{
    _fn = fn;
    _data = data;
}
