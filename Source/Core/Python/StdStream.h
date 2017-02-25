#ifndef __PYTHON_STD_STREAM_H__
#define __PYTHON_STD_STREAM_H__

#include <Python/Class.h>

namespace python_stdio
{
    typedef void(*TextCallback)(void* data, const char* msg);

    struct Stream
    {
        TextCallback cb;
        void* data;
    };

    void write(Stream* self, const char* text);
    void flush(Stream* self);
}
//
//class PyStdStream : public python::BaseObject
//{
//
//};


#endif // __PYTHON_STD_STREAM_H__
