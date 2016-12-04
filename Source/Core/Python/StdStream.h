#ifndef __PYTHON_STD_STREAM_H__
#define __PYTHON_STD_STREAM_H__

namespace py_std_stream
{
    typedef void (Callback)(void* data, const char* msg);

    void init_type();

    PyObject* create_stream();
    void set_callback(PyObject* self, Callback* fn, void* data);
}

#endif // __PYTHON_STD_STREAM_H__
