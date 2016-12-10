#include "Common.h"

#include "Object.h"
#include "PythonObject.h"
#include "Python/PythonType.h"
#include "Python/PythonCommon.h"


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
    if(self)
        ((PyPlutoObject*)self)->obj = obj;
}
Object* py_object::object(PyObject* self)
{
    if(self)
        return ((PyPlutoObject*)self)->obj;
    return nullptr;
}
