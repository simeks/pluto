#ifndef __PYTHON_MODULE_H__
#define __PYTHON_MODULE_H__

#include <Core/Python/Function.h>
#include <Core/Python/Object.h>

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
    void init_module_##Name##(python::Module const& module)

#define PYTHON_MODULE_INSTALL(name) \
    PyImport_AppendInittab(#name, ::PyInit_##name##);


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

    /// Imports and returns the specified module
    CORE_API Object import(const char* name);

    /// Reloads the specified module and returns a new reference to the reloaded module
    CORE_API Object reload_module(const Object& module);

    /// Returns the dictionary object belonging to the given module
    CORE_API Object get_dict(const Object& module);

}

#include "Module.inl"

#endif // __PYTHON_MODULE_H__
