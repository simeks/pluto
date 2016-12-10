#include "Common.h"

#include "StdStream.h"

#include "PythonType.h"
#include "Object/PythonObject.h"
#include "PythonCommon.h"


OBJECT_PYTHON_VARARGS_METHOD_IMPL(PyStdStream, write);
OBJECT_PYTHON_NOARGS_METHOD_IMPL(PyStdStream, flush);

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

PyObject* PyStdStream::write(const Tuple& args)
{
    if (args.size() >= 1) 
    {
        PyObject* obj = args.get(0);
        if (PyUnicode_Check(obj)) 
        {
            if (_fn)
            {
                _fn(_data, PyUnicode_AsUTF8(obj));
            }
        }
    }
    Py_RETURN_NONE;
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
