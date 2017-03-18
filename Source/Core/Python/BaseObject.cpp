#include "Common.h"

#include "BaseObject.h"

namespace python
{
    BaseObject::BaseObject() : _obj(nullptr)
    {
    }
    BaseObject::~BaseObject()
    {
    }
    void BaseObject::addref()
    {
        assert(_obj);
        Py_INCREF(_obj);
    }
    void BaseObject::release()
    {
        assert(_obj);
        Py_DECREF(_obj);
    }
    void BaseObject::initialize_python(PyObject* obj)
    {
        _obj = obj;
        _type = Py_TYPE(obj);
    }
}
