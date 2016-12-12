#include "Common.h"

#include "Object.h"

#include "Python/PythonCommon.h"

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(Object, object_type);

OBJECT_INIT_TYPE_FN(Object)
{
    OBJECT_PYTHON_ADD_METHOD(Object, object_type, METH_NOARGS, "");
}

IMPLEMENT_OBJECT(Object, "Object");

Object::Object() : _py_object(nullptr)
{
}
Object::~Object()
{
}
Object* Object::clone() const
{
    Object* clone = get_class()->create_object();
    copy_object_to(clone);
    return clone;
}
bool Object::is_a(Class* type) const
{
    if (type == nullptr)
        return true;

    for (Class* t = this->get_class(); t; t = t->super_class())
    {
        if (t == type)
            return true;
    }
    return false;
}

Object::Object(const Object& other)
{
    _py_object = other._py_object;
    Py_XINCREF(_py_object);
}
Object& Object::operator=(const Object& other)
{
    _py_object = other._py_object;
    Py_XINCREF(_py_object);

    return *this;
}
void Object::addref()
{
    validate();
    Py_XINCREF(_py_object);
}
void Object::release()
{
    validate();
    Py_XDECREF(_py_object);
}
PyObject* Object::object_type()
{
    return PyUnicode_FromString(get_class()->name());
}
PyObject* Object::invoke_method(const char* name, PyObject* args)
{
    PyObject* method = PyObject_GetAttrString(_py_object, name);
    if (!method)
    {
        PyErr_Print();
        return nullptr;
    }

    PyObject* ret = PyObject_Call(method, args, nullptr);
    if (!ret)
    {
        PyErr_Print();
        return nullptr;
    }
    return ret;
}
void Object::set_python_object(PyObject* obj)
{
    _py_object = obj;
}
void Object::validate()
{
    if (!_py_object)
    {
        _py_object = python_object();
    }
}

typedef struct {
    PyObject_HEAD
        Object* obj;
} PyPlutoObject;

PythonType* py_object::register_python_type(const char* name)
{
    PythonType* type = new PythonType(name, sizeof(PyPlutoObject));
    return type;
}
void py_object::set_object(PyObject* self, Object* obj)
{
    if (self)
        ((PyPlutoObject*)self)->obj = obj;
}
Object* py_object::object(PyObject* self)
{
    if (self)
        return ((PyPlutoObject*)self)->obj;
    return nullptr;
}