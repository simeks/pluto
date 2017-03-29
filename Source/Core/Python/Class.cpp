#include "Common.h"

#include "BaseObject.h"
#include "Class.h"
#include "Function.h"
#include "Module.h"
#include "PythonCommon.h"

#include <structmember.h>

using namespace python;

static PyObject* pluto_object_new(PyTypeObject* type, PyObject* , PyObject*)
{
    Instance* obj = (Instance*)type->tp_alloc(type, 0);
    obj->holder = nullptr;

    return (PyObject*)obj;
}
static int pluto_object_init(PyObject* , PyObject* , PyObject* )
{
    PyErr_SetString(PyExc_TypeError, "No constructor defined for C++ class.");
    return -1;
}
static void pluto_object_dealloc(PyObject* self)
{
    Instance* instance = (Instance*)self;
    delete instance->holder;

    Py_XDECREF(instance->dict);
    Py_TYPE(self)->tp_free(self);
}

static PyObject* pluto_object_get_dict(PyObject* self, void*)
{
    Instance* o = (Instance*)self;
    if (o->dict == 0)
        o->dict = PyDict_New();
    Py_INCREF(o->dict);
    return o->dict;
}

static int pluto_object_set_dict(PyObject* self, PyObject* dict, void*)
{
    Instance* o = (Instance*)self;
    Py_DECREF(o->dict);
    Py_INCREF(dict);
    o->dict = dict;
    return 0;
}

static PyGetSetDef pluto_object_getsets[] = {
    { "__dict__",  pluto_object_get_dict,  pluto_object_set_dict, NULL, 0 },
    { 0, 0, 0, 0, 0 }
};

static PyTypeObject _instance_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "instance",                 /*tp_name*/
    sizeof(Instance),           /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    pluto_object_dealloc,       /*tp_dealloc*/
    0,                          /*tp_print*/
    0,                          /*tp_getattr*/
    0,                          /*tp_setattr*/
    0,                          /*tp_compare*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
    0,                          /*tp_as_mapping*/
    0,                          /*tp_hash */
    0,                          /*tp_call*/
    0,                          /*tp_str*/
    0,                          /*tp_getattro*/
    0,                          /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    0,                          /* tp_doc */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    0,                          /* tp_methods */
    0,                          /* tp_members */
    pluto_object_getsets,       /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    pluto_object_init,          /* tp_init */
    PyType_GenericAlloc,        /* tp_alloc */
    pluto_object_new,           /* tp_new */
};

static const PyTypeObject& instance_type()
{
    static bool ready = false;
    if (!ready)
    {
        PyType_Ready(&_instance_type);
        ready = true;
    }
    return _instance_type;
}

namespace python
{
    Holder::Holder() {}
    Holder::~Holder() {}

    Class::Class()
    {
    }
    Class::Class(PyObject* obj) : Object(obj)
    {
        assert(PyType_Check(obj));
    }
    Class::Class(Borrowed obj) : Object(obj)
    {
        assert(PyType_Check(obj));
    }

    Class make_class(const char* name, PyTypeObject* base_type, const char* doc)
    {
        PyObject* bases = PyTuple_New(1);
        if (base_type)
            PyTuple_SetItem(bases, 0, incref((PyObject*)base_type));
        else
            PyTuple_SetItem(bases, 0, incref((PyObject*)&instance_type()));
        
        PyObject* args = PyTuple_New(3);
        PyTuple_SetItem(args, 0, PyUnicode_FromString(name));
        PyTuple_SetItem(args, 1, bases);
        PyTuple_SetItem(args, 2, incref(PyDict_New()));

        PyObject* cls = PyObject_Call((PyObject*)&PyType_Type, args, nullptr);
        Py_DECREF(args);
        
        if (doc)
        {
            PyObject* __doc__ = PyUnicode_FromString(doc);
            PyObject_SetAttrString(cls, "__doc__", __doc__);
            Py_DECREF(__doc__);
        }

        if (!cls)
            PyErr_Print();

        PyObject* mod = PyUnicode_FromString("pluto_builtins");
        PyObject_SetAttrString(cls, "__module__", mod);
        Py_DECREF(mod);

        return cls;
    }

    Object make_instance(PyTypeObject* type, Holder* holder)
    {
        Instance* obj = (Instance*)type->tp_alloc(type, 0);
        obj->holder = holder;
        return (PyObject*)obj;
    }

    void set_module(const Class& cls, const Module& module)
    {
        setattr(cls, "__module__", getattr(module, "__name__"));
    }

    void initialize_object(PyObject*, ...)
    {
    }
    void initialize_object(PyObject* obj, BaseObject* self)
    {
        self->initialize_python(obj);
    }
}
