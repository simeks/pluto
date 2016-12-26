#ifndef __OBJECT_PYTHON_H__
#define __OBJECT_PYTHON_H__

#include <Core/Object/Class.h>
#include <Core/Python/Dict.h>
#include <Core/Python/PythonWrapper.h>
#include <structmember.h>

class Tuple;

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
    typedef Object* (*CreateObjectFn)();

    PythonClass(const char* name, size_t size, CreateObjectFn creator);
    PythonClass(const char* name, PyTypeObject* type, PythonClass* super);
    ~PythonClass();

    Object* create_object(const Tuple& args);
    Object* create_object(PyObject* pyobj);
    
    void add_method(const char* name, PyCFunction meth, int flags, const char* doc = NULL);
    void add_member(const char* name, int type, int offset, int flags, const char* doc = NULL);
    void add_getset(const char* name, getter get, setter set, const char* doc, void* closure);
    void add_attr(const char* name, PyObject* obj, int flags = 0);

    void set_super(PythonClass* super);
    
    /// Returns Python class __dict__ entry
    Dict dict();

    PyObject* create_python_object(Object* owner);
    PyTypeObject* python_type();

    bool check_type(PyObject* obj);

    /// Indicates whetever this class is inherited from Python or a C++ class
    bool is_python() const;

    int ready();
    static void ready_all();
    static std::vector<PythonClass*> classes();

    /// Tries to find the PythonClass assosciated with the specified python type, if no PythonClass is found a new one is created.
    static PythonClass* python_class(PyTypeObject* type);

    /// Tries to find a class with the specified name, returns nullptr if no class with the given name was found.
    static PythonClass* python_class(const char* name);

private:
    CreateObjectFn _creator;

    PyTypeObject* _type;
    bool _owned; // Is the above type object owned by this classed or just a borrowed reference?

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
