#ifndef __PYTHON_CLASS_H__
#define __PYTHON_CLASS_H__

#include "Object.h"

#define PYTHON_CLASS(cls, name)

namespace python
{
    class Class;

    typedef void(*ClassInit)(const python::Class& cls);

    //class CORE_API InstanceFactory
    //{
    //public:
    //    InstanceFactory();
    //    virtual InstanceFactory();

    //    virtual void construct(PyObject* args, PyObject* kw) = 0;
    //    virtual void destruct() = 0;
    //};

    class CORE_API Class : public Object
    {
    public:
        Class(const char* name, ClassInit init_class);
        
    };

    template<typename T>
    Object make_class(const char* name, ClassInit init_class);

    /// @brief Returns the python class object for the specified class
    template<typename T>
    const Object& class_object();

    template<typename T>
    Object make_ref_instance(T* obj);

}

#include "Class.inl"

#endif // __PYTHON_CLASS_H__
