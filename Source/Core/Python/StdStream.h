#ifndef __PYTHON_STD_STREAM_H__
#define __PYTHON_STD_STREAM_H__

#include <Python/Class.h>

class PyStdStream
{
//public:
//    static const python::Class& python_class();

public:
    typedef void (Callback)(void* data, const char* msg);
    
    PyStdStream();
    ~PyStdStream();

    void write(const char* text);
    void flush();

    void set_callback(Callback* fn, void* data);

private:
    Callback* _fn;
    void* _data;
};

#endif // __PYTHON_STD_STREAM_H__
