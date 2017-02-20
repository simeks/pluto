//#include "Common.h"
//
//#include "Class.h"
//
//#include "Common.h"
//
//#include "Object.h"
//
//#include <structmember.h>
//
//typedef struct {
//    PyObject_HEAD;
//    PyObject* dict;
//    Object* obj;
//} PyPlutoObject;
//
//
//static PyObject* py_object_new(PyTypeObject* type, PyObject* args, PyObject*)
//{
//    PyObject* pyobj = type->tp_alloc(type, 0);
//    if (PyCapsule_CheckExact(args))
//    {
//        ((PyPlutoObject*)pyobj)->obj = (Object*)PyCapsule_GetPointer(args, "cpp_object");
//        return pyobj;
//    }
//
//    PythonClass* cls = nullptr;
//    PyTypeObject* base_type = type;
//    while (base_type != nullptr && cls == nullptr)
//    {
//        PyObject* dict = base_type->tp_dict;
//        if (dict)
//        {
//            PyObject* cap = PyDict_GetItemString(dict, "__cpp_class__");
//            if (cap)
//                cls = (PythonClass*)PyCapsule_GetPointer(cap, "cpp_class");
//        }
//        base_type = base_type->tp_base;
//    }
//
//    if (cls)
//    {
//        ((PyPlutoObject*)pyobj)->obj = cls->create_object(pyobj);
//    }
//    return pyobj;
//}
//static int py_object_init(PyObject* self, PyObject* arg, PyObject* kw)
//{
//    if (!PyCapsule_CheckExact(arg))
//    {
//        ((PyPlutoObject*)self)->obj->object_python_init(Tuple(arg), Dict(kw));
//        if (PyErr_Occurred())
//            return -1;
//    }
//    return 0;
//}
//static void py_object_dealloc(PyObject* self)
//{
//    Object* obj = ((PyPlutoObject*)self)->obj;
//    if (obj)
//    {
//        delete obj;
//    }
//}
//static PyObject* py_object_get_dict(PyObject* self, void*)
//{
//    PyPlutoObject* o = (PyPlutoObject*)self;
//    if (o->dict == 0)
//        o->dict = PyDict_New();
//    Py_INCREF(o->dict);
//    return o->dict;
//}
//
//static int py_object_set_dict(PyObject* self, PyObject* dict, void*)
//{
//    PyPlutoObject* o = (PyPlutoObject*)self;
//    Py_DECREF(o->dict);
//    Py_INCREF(dict);
//    o->dict = dict;
//    return 0;
//}
//static PyObject* py_object__copy__(PyPlutoObject* self, PyObject*, PyObject*)
//{
//    Object* copy = object_clone(self->obj);
//    return copy->python_object();
//}
//
//
//Object* python_object::object(PyObject* pyobj)
//{
//    if (pyobj)
//        return ((PyPlutoObject*)pyobj)->obj;
//    return nullptr;
//}
//PyObject* python_object::copy(PyObject* pyobj, Object* owner)
//{
//    PyTypeObject* type = pyobj->ob_type;
//
//    PyObject* owner_capsule = nullptr;
//    if (owner)
//        owner_capsule = PyCapsule_New(owner, "cpp_object", nullptr);
//
//    PyObject* copy = type->tp_new(type, owner_capsule, nullptr);
//    PyObject* src_dict = py_object_get_dict(pyobj, nullptr);
//    PyObject* dst_dict = py_object_get_dict(copy, nullptr);
//    PyDict_Update(dst_dict, src_dict);
//    Py_DECREF(src_dict);
//    Py_DECREF(dst_dict);
//
//    return copy;
//}
//
//
//
//static PyTypeObject py_blank_type_object = {
//    PyVarObject_HEAD_INIT(NULL, 0)
//    0,             /*tp_name*/
//    0,             /*tp_basicsize*/
//    0,                         /*tp_itemsize*/
//    0, /*tp_dealloc*/
//    0,                         /*tp_print*/
//    0,                         /*tp_getattr*/
//    0,                         /*tp_setattr*/
//    0,           /*tp_compare*/
//    0,              /*tp_repr*/
//    0,                         /*tp_as_number*/
//    0,                         /*tp_as_sequence*/
//    0,                         /*tp_as_mapping*/
//    0,                         /*tp_hash */
//    0,                         /*tp_call*/
//    0,                         /*tp_str*/
//    0,                         /*tp_getattro*/
//    0,                         /*tp_setattro*/
//    0,                         /*tp_as_buffer*/
//    0, /*tp_flags*/
//    0,           /* tp_doc */
//    0,                   /* tp_traverse */
//    0,                   /* tp_clear */
//    0,                   /* tp_richcompare */
//    0,                   /* tp_weaklistoffset */
//    0,                   /* tp_iter */
//    0,                   /* tp_iternext */
//    0,                   /* tp_methods */
//    0,                   /* tp_members */
//    0,                   /* tp_getset */
//    0,                         /* tp_base */
//    0,                         /* tp_dict */
//    0,                         /* tp_descr_get */
//    0,                         /* tp_descr_set */
//    0,                         /* tp_dictoffset */
//    0,                         /* tp_init */
//    0,                         /* tp_alloc */
//    0,                 /* tp_new */
//};
//
