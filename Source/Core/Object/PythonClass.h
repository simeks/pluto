#ifndef __OBJECT_PYTHON_H__
#define __OBJECT_PYTHON_H__

#include <Core/Object/Class.h>
#include <Core/Python/PythonWrapper.h>
#include <structmember.h>

namespace python_object
{
    CORE_API Object* object(PyObject* pyobj);

    /// Copies the specified PyObject, 
    /// owner : Specifies the owner of the new object, if null a new owner object is created 
    CORE_API PyObject* copy(PyObject* pyobj, Object* owner = nullptr);
}

class CORE_API PythonClass : public Class
{
public:
    enum AttributeFlags
    {
        Attr_NoFlags = 0,
        Attr_Public = 1
    };
    PythonClass(const char* name, size_t size, CreateObjectFn creator);
    ~PythonClass();

    
    void add_method(const char* name, PyCFunction meth, int flags, const char* doc = NULL);
    void add_member(const char* name, int type, int offset, int flags, const char* doc = NULL);
    void add_getset(const char* name, getter get, setter set, const char* doc, void* closure);
    void add_attr(const char* name, PyObject* obj, int flags = 0);

    void set_super(PythonClass* super);
    
    PyObject* create_python_object(Object* owner);
    PyTypeObject* python_type();

    bool check_type(PyObject* obj);

    int ready();
    static void ready_all();

private:
    PyTypeObject _type;
    PyObject* _dict;

    std::vector<PyMethodDef> _methods;
    std::vector<PyMemberDef> _members;
    std::vector<PyGetSetDef> _getsets;

    static PythonClass* _head;
    PythonClass* _next;
};

template <class T, typename std::enable_if<std::is_base_of<Object, T>::value, Object>::type* = nullptr>
T* pyobject_extract_instance(PyObject* self)
{
    return (T*)python_object::object(self);
}


#endif // __OBJECT_PYTHON_H__
