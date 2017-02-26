#include "Common.h"

#include <Core/Python/Convert.h>
#include <Core/Python/Function.h>
#include "StdStream.h"
//
//void init_python_class_StdStream(const python::Class& cls);
//namespace python
//{
//    template<>
//    const Object& class_object<python_stdio::Stream>()
//    {
//        static Object cls;
//        if (cls.is_none())
//        {
//            cls = python::Class("StdStream", &::init_python_class_StdStream);
//        }
//        return cls;
//    }
//    template<>
//    CORE_API python_stdio::Stream* from_python(const python::Object& obj)
//    {
//        if (obj.ptr() == Py_None)
//            return nullptr;
//
//        if (obj.is_instance(class_object<python_stdio::Stream>()))
//        {
//
//        }
//        PyErr_SetString(PyExc_ValueError, "Failed to convert Object");
//        return nullptr;
//    }
//    template<>
//    CORE_API python::Object to_python(python_stdio::Stream* const& obj)
//    {
//        obj;
//
//        PyErr_SetString(PyExc_ValueError, "Failed to convert Object");
//        return python::None();
//    }
//    //template<>
//    //CORE_API python_stdio::Stream& from_python(const python::Object& obj)
//    //{
//    //    if (obj.is_instance(class_object<python_stdio::Stream>()))
//    //    {
//
//    //    }
//    //    PyErr_SetString(PyExc_ValueError, "Failed to convert Object");
//    //    return python_stdio::Stream();
//    //}
//    //template<>
//    //CORE_API python::Object to_python(const python_stdio::Stream& obj)
//    //{
//    //    obj;
//
//    //    PyErr_SetString(PyExc_ValueError, "Failed to convert Object");
//    //    return python::None();
//    //}
//}
//void init_python_class_StdStream(const python::Class& cls)
//{
//    python::def(cls, "write", &python_stdio::write);
//    python::def(cls, "flush", &python_stdio::flush);
//}

void python_stdio::write(Stream* self, const char* text)
{
    if (self && self->cb)
        self->cb(self->data, text);
}
void python_stdio::flush(Stream*)
{
    // Do nothing
}
