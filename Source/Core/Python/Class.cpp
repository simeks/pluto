#include "Common.h"

#include "Class.h"
#include "Function.h"

#include <structmember.h>


typedef struct {
    PyObject_HEAD;
    PyObject* dict;
} PlutoObject;

static PyObject* pluto_object_get_dict(PyObject* self, void*)
{
    PlutoObject* o = (PlutoObject*)self;
    if (o->dict == 0)
        o->dict = PyDict_New();
    Py_INCREF(o->dict);
    return o->dict;
}

static int pluto_object_set_dict(PyObject* self, PyObject* dict, void*)
{
    PlutoObject* o = (PlutoObject*)self;
    Py_DECREF(o->dict);
    Py_INCREF(dict);
    o->dict = dict;
    return 0;
}

static PyGetSetDef pluto_object_getsets[] = {
    { "__dict__",  pluto_object_get_dict,  pluto_object_set_dict, NULL, 0 },
    { 0, 0, 0, 0, 0 }
};

static PyTypeObject py_blank_type_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "PlutoObject",               /*tp_name*/
    sizeof(PlutoObject),        /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    0,                          /*tp_dealloc*/
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                          /*tp_flags*/
    0,                          /* tp_doc */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    0,                          /* tp_methods */
    0,                          /* tp_members */
    pluto_object_getsets,           /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    0,                          /* tp_init */
    0,                          /* tp_alloc */
    0,                          /* tp_new */
};


namespace python
{
    Object new_class(const char* name)
    {
        PyObject* bases = PyTuple_New(1);
        PyTuple_SetItem(bases, 0, incref((PyObject*)&PyBaseObject_Type));
        
        PyObject* args = PyTuple_New(3);
        PyTuple_SetItem(args, 0, PyUnicode_FromString(name));
        PyTuple_SetItem(args, 1, bases);
        PyTuple_SetItem(args, 2, incref(Dict().dict()));

        PyObject* cls = PyObject_Call((PyObject*)&PyType_Type, args, nullptr);
        if (!cls)
            PyErr_Print();

        Py_DECREF(args);
        
        return cls;
    }

    Class::Class(const char* name, ClassInit ) : Object(new_class(name)) //TODO: Leaks a reference
    {
    }
}
