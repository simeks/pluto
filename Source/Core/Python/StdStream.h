#ifndef __PYTHON_STD_STREAM_H__
#define __PYTHON_STD_STREAM_H__

#include <Core/Python/Class.h>

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

    python::Object stream_class();
}


#endif // __PYTHON_STD_STREAM_H__
