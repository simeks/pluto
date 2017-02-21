#ifndef __PYTHON_CLASS_H__
#define __PYTHON_CLASS_H__

#include "Object.h"

#define PYTHON_CLASS(cls, name)

namespace python
{
    class Class;

    typedef void(*ClassInit)(const python::Class& cls);

    class CORE_API Class : public Object
    {
    public:
        Class(const char* name, ClassInit init_class);
        
    };
}

#include "Class.inl"

#endif // __PYTHON_CLASS_H__
