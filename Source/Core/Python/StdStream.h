#ifndef __PYTHON_STD_STREAM_H__
#define __PYTHON_STD_STREAM_H__

#include <Object/Object.h>

class PyStdStream : public Object
{
    DECLARE_OBJECT(PyStdStream, Object);

public:
    typedef void (Callback)(void* data, const char* msg);
    
    PyStdStream();
    ~PyStdStream();

    PyObject* write(const Tuple& args);
    PyObject* flush();

    void set_callback(Callback* fn, void* data);

private:
    Callback* _fn;
    void* _data;
};

#endif // __PYTHON_STD_STREAM_H__
