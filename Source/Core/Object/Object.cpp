#include "Common.h"

#include "Object.h"

#include <Core/Python/Class.h>


PLUTO_OBJECT_IMPL(Object, "Object")
{
    cls;
}

Object::Object() : _obj(nullptr), _type(nullptr)
{
}
Object::~Object()
{
}
bool Object::has_attribute(const char* name) const
{
    assert(_obj);
    return PyObject_HasAttrString(_obj, name) == 1;
}
python::Object Object::attribute(const char* name) const
{
    assert(_obj);
    PyObject* attr = PyObject_GetAttrString(_obj, name);
    if (!attr)
    {
        PyErr_Print(); // TODO: Error handling
        return python::Object(Py_None); // TODO: Return None or throw exception?
    }
    return attr;
}
void Object::set_attribute(const char* name, const python::Object& obj)
{
    assert(_obj);
    if (PyObject_SetAttrString(_obj, name, obj.ptr()) != 0)
        PyErr_Print();
}
bool Object::is_a(const python::Class& type) const
{
    if (type.is_none())
        return false;

    if (PyObject_IsInstance(_obj, type.ptr()))
        return true;

    return false;
}
bool Object::is_overridden(const char* name) const
{
    assert(_obj);
    PyObject* method = PyObject_GetAttrString(_obj, name);
    if (method)
    {
        if (PyMethod_Check(method))
        {
            // We can assume that the function is not overridden (at least not by python)
            //  if the function is of the type defined in Function.cpp.

            if (PyFunction_Check(PyMethod_Function(method)))
                return true;
        }
    }
    return false;
}
bool Object::is_overridden(const python::Object& method) const
{
    if (PyMethod_Check(method.ptr()))
    {
        // We can assume that the function is not overridden (at least not by python)
        //  if the function is of the type defined in Function.cpp.

        if (PyFunction_Check(PyMethod_Function(method.ptr())))
            return true;
    }
        
    return false;
}
void Object::addref()
{
    assert(_obj);
    Py_INCREF(_obj);
}
void Object::release()
{
    assert(_obj);
    Py_DECREF(_obj);
}
PyObject* Object::ptr() const
{
    return _obj;
}
void Object::initialize_python(PyObject* obj)
{
    _obj = obj;
    _type = Py_TYPE(obj);
}

Object::Object(const Object& other)
{
    _type = other._type;

    PyObject* copy = _type->tp_new(_type, nullptr, nullptr);
    // Using PtrHolder<Object> should suffice as all classes here should subclass Object
    // However, this assumes PtrHolder is just a "holder" that does not perform any magic depending on type
    ((python::Instance*)copy)->holder = new python::PtrHolder<Object>(this, true);

    PyObject* src_dict = PyObject_GetAttrString(other._obj, "__dict__");
    PyObject* dst_dict = PyObject_GetAttrString(copy, "__dict__");
    PyDict_Update(dst_dict, src_dict);
    Py_DECREF(src_dict);
    Py_DECREF(dst_dict);

    _obj = copy; // Object will allocated by the cloned class
}
void initialize_object(PyObject*, ...)
{
}
void initialize_object(PyObject* obj, Object* self)
{
    self->initialize_python(obj);
}
