#include "Common.h"

#include <Core/Python/Convert.h>
#include <Core/Python/Function.h>
#include "StdStream.h"

namespace python
{
    Stream::Stream() : _cb(nullptr), _data(nullptr)
    {
    }
    void Stream::write(const char* text)
    {
        if (_cb)
            _cb(_data, text);
    }
    void Stream::flush()
    {
        // Do nothing
    }
    void Stream::set_callback(Callback cb, void* data)
    {
        _cb = cb;
        _data = data;
    }
    Object Stream::stream_class()
    {
        static Object cls;
        if (cls.ptr() == Py_None)
        {
            cls = make_class<Stream>("Stream");
            def(cls, "write", &Stream::write);
            def(cls, "flush", &Stream::flush);
        }
        return cls;
    }
}
