#ifndef __PYTHON_MODULE_H__
#define __PYTHON_MODULE_H__

#include "Object.h"

#define PYTHON_MODULE(Name) \
    void init_module_##Name##(python::Module const& m); \
    PyObject* PyInit_##Name##() \
    { \
        static struct PyModuleDef s_##Name##_module_def = { \
            PyModuleDef_HEAD_INIT, \
            #Name,   /* m_name */ \
            "", /* m_doc */ \
            -1,       /* m_size */ \
            0, /* m_methods */ \
            0,  /* m_reload */ \
            0, /* m_traverse */ \
            0, /* m_clear */ \
            0,  /* m_free */ \
        }; \
        PyObject* m = PyModule_Create(&s_##Name##_module_def); \
        if (!m) \
            PyErr_Print(); \
        else \
            init_module_##Name##(python::Module(m)); \
        return m; \
    } \
    void init_module_##Name##(python::Module const& m)

#define PYTHON_MODULE_INSTALL(name) \
    extern PyObject* PyInit_##name##(); \
    PyImport_AppendInittab(#name, ::PyInit_##name##);


#define PYTHON_MODULE_FUNCTION(name, fn) python::setattr(m, name, python::make_function(fn));
#define PYTHON_MODULE_CLASS(name, cls) python::setattr(m, name, (PyObject*)cls::static_class()->python_type());

namespace python
{
    /// Module object wrapper
    class CORE_API Module : public Object
    {
    public:
        explicit Module(PyObject* m);

    private:
        PyObject* _m;
    };
}

#endif // __PYTHON_MODULE_H__
