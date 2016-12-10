#ifndef __CORE_PYTHON_OBJECT_H__
#define __CORE_PYTHON_OBJECT_H__

class Class;
class Object;
class PythonType;

namespace py_object
{
    PythonType* register_python_type(const char* name);

    void set_object(PyObject* self, Object* obj);
    Object* object(PyObject* self);

}

#endif // __CORE_PYTHON_OBJECT_H__
