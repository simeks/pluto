#include "Common.h"


#include "PythonCommon.h"

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

PythonType* PythonType::_head = nullptr;

PythonType::PythonType(const char* name, size_t basic_size) : _next(nullptr)
{
    if(_head)
    {
        PythonType* t = _head;
        while (t->_next) t = t->_next;
        t->_next = this;
    }
    else
    {
        _head = this;
    }

    memcpy(&_type, &py_blank_type_object, sizeof(py_blank_type_object));
    _type.tp_name = name;
    _type.tp_basicsize = basic_size;
    _type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    _type.tp_doc = "PythonType";

    PyMethodDef sentinel = { NULL, NULL, 0, NULL };
    _methods.push_back(sentinel); // Add sentinel
}
PythonType::~PythonType()
{
}
void PythonType::add_method(const char *name, PyCFunction meth, int flags, const char *doc)
{
    PyMethodDef def;
    def.ml_name = name;
    def.ml_meth = meth;
    def.ml_flags = flags;
    def.ml_doc = doc;
    _methods.insert(_methods.end() - 1, def); // Insert before sentinel
}
void PythonType::set_new(newfunc fn)
{
    _type.tp_new = fn;
}
void PythonType::set_init(initproc fn)
{
    _type.tp_init = fn;
}
void PythonType::set_dealloc(destructor fn)
{
    _type.tp_dealloc = fn;
}
void PythonType::set_base(PythonType* base)
{
    if(base != this)
        _type.tp_base = base->type_object();
}
int PythonType::ready()
{
    _type.tp_methods = _methods.data();
    return PyType_Ready(&_type);
}
PyObject* PythonType::create_object(PyObject* args, PyObject* kw)
{
    PyObject* ret = PyObject_Call((PyObject*)&_type, args, kw);
    if (!ret)
        PyErr_Print();
    return ret;
}
bool PythonType::check_type(PyObject* instance)
{
    return PyObject_IsInstance(instance, (PyObject*)&_type) != 0;
}
const char* PythonType::name() const
{
    return _type.tp_name;
}
PyTypeObject* PythonType::type_object()
{
    return &_type;
}

void PythonType::ready_all()
{
    PythonType* t = _head;
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
