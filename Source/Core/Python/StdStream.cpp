#include "Common.h"

#include "StdStream.h"

#include "PythonCommon.h"


PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(PyStdStream, write, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0(PyStdStream, flush);

OBJECT_INIT_TYPE_FN(PyStdStream)
{
    OBJECT_PYTHON_ADD_METHOD(PyStdStream, write, "");
    OBJECT_PYTHON_ADD_METHOD(PyStdStream, flush, "");
}

IMPLEMENT_OBJECT(PyStdStream, "StdStream", CORE_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(PyStdStream, Object);

PyStdStream::~PyStdStream()
{
}

void PyStdStream::object_init()
{
    _fn = nullptr;
    _data = nullptr;
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
