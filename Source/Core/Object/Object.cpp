#include "Common.h"

#include "Object.h"

#include "Python/PythonCommon.h"
#include "Python/PythonFunction.h"

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(Object, object_type);

OBJECT_INIT_TYPE_FN(Object)
{
    OBJECT_PYTHON_ADD_METHOD(Object, object_type, "");
}

IMPLEMENT_OBJECT(Object, "Object", CORE_API);

Object::Object(PyObject* pyobj, PythonClass* cls) : _class(cls), _py_object(pyobj)
{
    if (!_py_object)
    {
        _py_object = _class->create_python_object(this);
    }
}
Object::Object(const Object& other)
{
    // Copy pyobj
    _class = other._class;
    _py_object = python_object::copy(other._py_object, this);
}
Object::~Object()
{
}
bool Object::is_a(Class* type)
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

void Object::addref()
{
    Py_XINCREF(_py_object);
}
void Object::release()
{
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
    Py_DECREF(method);
    return ret;
}
void Object::set_attribute(const char* name, const python::Object& attr)
{
    if (PyObject_SetAttrString(_py_object, name, attr.ptr()) != 0)
        PyErr_Print();
}
bool Object::has_attribute(const char* name) const
{
    return PyObject_HasAttrString(_py_object, name) == 1;
}
PyObject* Object::attribute(const char* name) const
{
    return PyObject_GetAttrString(_py_object, name);
}
PyObject* Object::python_object()
{
    return _py_object;
}
void Object::set_class(PythonClass* cls)
{
    _class = cls;
}


