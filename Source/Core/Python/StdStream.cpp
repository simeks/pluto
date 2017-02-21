#include "Common.h"

#include "StdStream.h"

//
//PYTHON_CLASS(PyStdStream, "StdStream")

//
//const python::Class& PyStdStream::python_class()
//{
//    static python::Class cls = python::make_class<PyStdStream>("StdStream", &init_python_class);
//    return cls;
//}
//void init_python_class(const python::Class& cls)
//{
//    python::def(cls, "write", &PyStdStream::write);
//    python::def(cls, "flush", &PyStdStream::flush);
//}


PyStdStream::PyStdStream()
{
    _fn = nullptr;
    _data = nullptr;
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
void PyStdStream::flush()
{
}
void PyStdStream::set_callback(Callback* fn, void* data)
{
    _fn = fn;
    _data = data;
}
