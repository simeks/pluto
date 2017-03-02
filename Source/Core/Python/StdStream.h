#ifndef __PYTHON_STD_STREAM_H__
#define __PYTHON_STD_STREAM_H__

#include <Core/Python/Class.h>

namespace python
{
    class Stream
    {
    public:
        typedef void(*Callback)(void*, const char*);

        Stream();

        void write(const char* text);
        void flush();

        void set_callback(Callback cb, void* data);

        static Object stream_class();

    private:
        Callback _cb;
        void* _data;
    };
}

#endif // __PYTHON_STD_STREAM_H__
