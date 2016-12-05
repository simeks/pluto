#include "Common.h"

#include "StdStream.h"

#include "PythonWrapper.h"

typedef struct {
    PyObject_HEAD
    py_std_stream::Callback* _fn;
    void* _data;
} PythonStdStream;


static PyObject *PythonStdStream_new(PyTypeObject* type, PyObject * /*args*/, PyObject * /*kwds*/)
{
    PythonStdStream* self = (PythonStdStream*)type->tp_alloc(type, 0);
    self->_fn = nullptr;
    self->_data = nullptr;

    return (PyObject *)self;
}

static PyObject *PythonStdStream_write(PythonStdStream* self, PyObject *args)
{
    if (PyTuple_GET_SIZE(args) >= 1) {
        PyObject* obj = PyTuple_GET_ITEM(args, 0);
        if (PyUnicode_Check(obj)) {
            if (self->_fn)
            {
                self->_fn(self->_data, PyUnicode_AsUTF8(obj));
            }
        }
    }

    Py_RETURN_NONE;
}
static PyObject *PythonStdStream_flush(PythonStdStream* /*self*/, PyObject* /*args*/)
{
    Py_RETURN_NONE;
}

static PyMethodDef PythonStdStream_methods[] = {
    { "write", (PyCFunction)PythonStdStream_write, METH_VARARGS, "" },
    { "flush", (PyCFunction)PythonStdStream_flush, METH_VARARGS, "" },
    { NULL,    NULL, 0 , NULL } /* sentinel */
};

static PyTypeObject PythonStdStreamType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "PythonStdStream",             /*tp_name*/
    sizeof(PythonStdStream),             /*tp_basicsize*/
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PythonStdStream",           /* tp_doc */
    0,                   /* tp_traverse */
    0,                   /* tp_clear */
    0,                   /* tp_richcompare */
    0,                   /* tp_weaklistoffset */
    0,                   /* tp_iter */
    0,                   /* tp_iternext */
    PythonStdStream_methods,                   /* tp_methods */
    0,                   /* tp_members */
    0,                   /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PythonStdStream_new,                 /* tp_new */
};

void py_std_stream::init_type()
{
    if (PyType_Ready(&PythonStdStreamType) < 0)
    {
        // TODO: Error
        return;
    }

    Py_INCREF(&PythonStdStreamType);
}

PyObject* py_std_stream::create_stream()
{
    PyObject* obj = PythonStdStreamType.tp_new(&PythonStdStreamType, nullptr, nullptr);
    if (!obj)
        PyErr_Print();

    return obj;
}
void py_std_stream::set_callback(PyObject* self, Callback* fn, void* data)
{
    if (PyObject_TypeCheck(self, &PythonStdStreamType))
    {
        ((PythonStdStream*)self)->_fn = fn;
        ((PythonStdStream*)self)->_data = data;
    }
}
