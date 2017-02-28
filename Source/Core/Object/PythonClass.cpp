#include "Common.h"

#include "Object.h"
#include "PythonClass.h"

#include <structmember.h>

typedef struct {
    PyObject_HEAD;
    PyObject* dict;
    Object* obj;
} PyPlutoObject;


static PyObject* py_object_new(PyTypeObject* type, PyObject* args, PyObject*)
{
    PyObject* pyobj = type->tp_alloc(type, 0);
    if (PyCapsule_CheckExact(args))
    {
        ((PyPlutoObject*)pyobj)->obj = (Object*)PyCapsule_GetPointer(args, "cpp_object");
        return pyobj;
    }

    PythonClass* cls = nullptr;
    PyTypeObject* base_type = type;
    while (base_type != nullptr && cls == nullptr)
    {
        PyObject* dict = base_type->tp_dict;
        if (dict)
        {
            PyObject* cap = PyDict_GetItemString(dict, "__cpp_class__");
            if (cap)
                cls = (PythonClass*)PyCapsule_GetPointer(cap, "cpp_class");
        }
        base_type = base_type->tp_base;
    }

    if (cls)
    {
        ((PyPlutoObject*)pyobj)->obj = cls->create_object(pyobj);
    }
    return pyobj;
}
static int py_object_init(PyObject* self, PyObject* arg, PyObject* kw)
{
    if (!PyCapsule_CheckExact(arg))
    {
        ((PyPlutoObject*)self)->obj->object_python_init(Tuple(arg), kw ? Dict(kw) : Dict());
        if (PyErr_Occurred())
            return -1;
    }
    return 0;
}
static void py_object_dealloc(PyObject* self)
{
    Object* obj = ((PyPlutoObject*)self)->obj;
    if (obj)
    {
        delete obj;
    }
}
static PyObject* py_object_get_dict(PyObject* self, void*)
{
    PyPlutoObject* o = (PyPlutoObject*)self;
    if (o->dict == 0)
        o->dict = PyDict_New();
    Py_INCREF(o->dict);
    return o->dict;
}

static int py_object_set_dict(PyObject* self, PyObject* dict, void*)
{
    PyPlutoObject* o = (PyPlutoObject*)self;
    Py_DECREF(o->dict);
    Py_INCREF(dict);
    o->dict = dict;
    return 0;
}
static PyObject* py_object__copy__(PyPlutoObject* self, PyObject*, PyObject*)
{
    Object* copy = object_clone(self->obj);
    return copy->python_object();
}


Object* python_object::object(PyObject* pyobj)
{
    if (pyobj)
        return ((PyPlutoObject*)pyobj)->obj;
    return nullptr;
}
PyObject* python_object::copy(PyObject* pyobj, Object* owner)
{
    PyTypeObject* type = pyobj->ob_type;

    PyObject* owner_capsule = nullptr;
    if (owner)
        owner_capsule = PyCapsule_New(owner, "cpp_object", nullptr);

    PyObject* copy = type->tp_new(type, owner_capsule, nullptr);
    PyObject* src_dict = py_object_get_dict(pyobj, nullptr);
    PyObject* dst_dict = py_object_get_dict(copy, nullptr);
    PyDict_Update(dst_dict, src_dict);
    Py_DECREF(src_dict);
    Py_DECREF(dst_dict);

    return copy;
}



static PyTypeObject py_blank_type_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    0,             /*tp_name*/
    0,             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,           /*tp_compare*/
    0,              /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    0, /*tp_flags*/
    0,           /* tp_doc */
    0,                   /* tp_traverse */
    0,                   /* tp_clear */
    0,                   /* tp_richcompare */
    0,                   /* tp_weaklistoffset */
    0,                   /* tp_iter */
    0,                   /* tp_iternext */
    0,                   /* tp_methods */
    0,                   /* tp_members */
    0,                   /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    0,                 /* tp_new */
};

PythonClass* PythonClass::_head = nullptr;

PythonClass::PythonClass(const char* name, size_t size, CreateObjectFn creator, InitClassFn initfn, const char* doc) :
    Class(name, size),
    _creator(creator),
    _initfn(initfn),
    _owned(true),
    _next(nullptr)
{
    if (_head)
    {
        PythonClass* t = _head;
        while (t->_next) t = t->_next;
        t->_next = this;
    }
    else
    {
        _head = this;
    }

    _type = (PyTypeObject*)malloc(sizeof(py_blank_type_object));
    memcpy(_type, &py_blank_type_object, sizeof(py_blank_type_object));
    
    _type->tp_name = name;
    _type->tp_basicsize = sizeof(PyPlutoObject);
    _type->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    _type->tp_doc = doc;

    PyMethodDef meth_sentinel = { 0, 0, 0, 0 };
    _methods.push_back(meth_sentinel); // Add sentinel

    PyMemberDef memb_sentinel = { 0, 0, 0, 0, 0 };
    _members.push_back(memb_sentinel); // Add sentinel

    PyGetSetDef getset_sentinel = { 0, 0, 0, 0, 0 };
    _getsets.push_back(getset_sentinel); // Add sentinel

    _type->tp_new = py_object_new;
    _type->tp_init = py_object_init;
    _type->tp_dealloc = py_object_dealloc;
    _type->tp_dictoffset = offsetof(PyPlutoObject, dict);

    add_getset("__dict__", py_object_get_dict, py_object_set_dict, 0, 0);
    add_method("__copy__", (PyCFunction)py_object__copy__, METH_NOARGS, "");

    _dict = PyDict_New();
}
PythonClass::PythonClass(const char* name, PyTypeObject* type, PythonClass* super) :
    Class(name, super->size()),
    _creator(super->_creator),
    _initfn(nullptr),
    _type(type),
    _owned(false),
    _next(nullptr)
{
    if (_head)
    {
        PythonClass* t = _head;
        while (t->_next) t = t->_next;
        t->_next = this;
    }
    else
    {
        _head = this;
    }
    set_super(super);

    _dict = type->tp_dict;
    Py_XINCREF(_dict);

    PyDict_SetItemString(_dict, "__cpp_class__", PyCapsule_New(this, "cpp_class", nullptr));
}
PythonClass::~PythonClass()
{
    Py_XDECREF(_dict);
    
    if (_owned)
        free(_type);
    else
        Py_DECREF(_type);
}
Object* PythonClass::create_object()
{
    return _creator(nullptr, this);
}
Object* PythonClass::create_object(const Tuple& args)
{
    PyObject* ret = PyObject_Call((PyObject*)_type, args.ptr(), 0);
    if (!ret)
    {
        return nullptr;
    }
    Object* obj = ((PyPlutoObject*)ret)->obj;
    obj->set_class(this);
    return obj;
}
Object* PythonClass::create_object(PyObject* pyobj)
{
    return _creator(pyobj, this);
}
void PythonClass::add_method(const char *name, PyCFunction meth, int flags, const char *doc)
{
    PyMethodDef def;
    def.ml_name = name;
    def.ml_meth = meth;
    def.ml_flags = flags;
    def.ml_doc = doc;
    _methods.insert(_methods.end() - 1, def); // Insert before sentinel
}
void PythonClass::add_member(const char *name, int type, int offset, int flags, const char *doc)
{
    PyMemberDef def = { const_cast<char*>(name), type, offset, flags, const_cast<char*>(doc) };
    _members.insert(_members.end() - 1, def); // Insert before sentinel
}
void PythonClass::add_getset(const char* name, getter get, setter set, const char* doc, void* closure)
{
    PyGetSetDef def = { const_cast<char*>(name), get, set, const_cast<char*>(doc), closure };
    _getsets.insert(_getsets.end() - 1, def); // Insert before sentinel
}
void PythonClass::add_attr(const char* name, PyObject* obj)
{
    PyDict_SetItemString(_dict, name, obj);
}
void PythonClass::set_super(PythonClass* super)
{
    if (this != super)
    {
        Class::set_super(super);
        _type->tp_base = super->_type;
    }
}
Dict PythonClass::dict()
{
    return Dict(_dict);
}
PyObject* PythonClass::create_python_object(Object* owner)
{
    PyObject* ret = PyObject_Call((PyObject*)_type, PyCapsule_New(owner, "cpp_object", 0), 0);
    if (!ret)
        PyErr_Print();
    return ret;
}
PyTypeObject* PythonClass::python_type()
{
    return _type;
}
bool PythonClass::check_type(PyObject* obj)
{
    return PyObject_IsInstance(obj, (PyObject*)_type) != 0;
}
bool PythonClass::is_python() const
{
    return !_owned;
}
int PythonClass::ready()
{
    if (_type->tp_flags & Py_TPFLAGS_READY) // Already initialzied
        return 0;

    if (_initfn)
        _initfn(this);

    _type->tp_methods = _methods.data();
    _type->tp_members = _members.data();
    _type->tp_getset = _getsets.data();
    int r = PyType_Ready(_type);

    PyObject* dict = _type->tp_dict;
    assert(dict);

    PyDict_SetItemString(dict, "__cpp_class__", PyCapsule_New(this, "cpp_class", nullptr));
    PyDict_Merge(dict, _dict, 1);

    Py_INCREF(dict);
    Py_DECREF(_dict);
    _dict = dict;
    
    return r;
}
void PythonClass::ready_all()
{
    PythonClass* t = _head;
    while (t)
    {
        if (t->ready() < 0)
        {
            // TODO: Something went wrong
            std::cout << "Failed to ready type '" << t->name() << "'." << std::endl;
        }
        t = t->_next;
    }
}
void PythonClass::destroy_all()
{
    PythonClass* t = _head;
    while (t)
    {
        PythonClass* next = t->_next;
        delete t;

        t = next;
    }
    _head = nullptr;
}
std::vector<PythonClass*> PythonClass::classes()
{
    std::vector<PythonClass*> v;
    PythonClass* t = _head;
    while (t)
    {
        v.push_back(t);
        t = t->_next;
    }
    return v;
}
PythonClass* PythonClass::python_class(PyTypeObject* type)
{
    if (!type)
        return nullptr;

    PythonClass* t = _head;
    while (t)
    {
        if (t->python_type() == type)
        {
            return t;
        }
        t = t->_next;
    }

    t = new PythonClass(type->tp_name, type, python_class(type->tp_base));
    return t;
}
PythonClass* PythonClass::python_class(const char* name)
{
    PythonClass* t = _head;
    while (t)
    {
        if (strcmp(t->name(), name) == 0)
        {
            return t;
        }
        t = t->_next;
    }
    return nullptr;
}
