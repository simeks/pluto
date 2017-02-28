#include "Common.h"

#include <Core/Python/Convert.h>
#include <Core/Python/Function.h>
#include "StdStream.h"

void python_stdio::write(Stream* self, const char* text)
{
    if (self && self->cb)
        self->cb(self->data, text);
}
void python_stdio::flush(Stream*)
{
    // Do nothing
}
python::Object python_stdio::stream_class()
{
    static python::Object cls;
    if (cls.ptr() == Py_None)
    {
        cls = python::make_class<Stream>("Stream");
        python::def(cls, "write", &write);
        python::def(cls, "flush", &flush);
    }
    return cls;
}
