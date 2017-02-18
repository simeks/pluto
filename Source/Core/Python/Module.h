#ifndef __PYTHON_MODULE_H__
#define __PYTHON_MODULE_H__

#include "Function.h"
#include "Object.h"

#include <Core/Object/PythonClass.h>

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
    extern PyObject* PyInit_##name##(); \
    PyImport_AppendInittab(#name, ::PyInit_##name##);


class PythonClass;

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

    /// @brief Adds a regular function the the given module
    template<typename TReturn, typename ... TArgs>
    void def(const Module& m, const char* name, TReturn(*fn)(TArgs...), const char* doc = nullptr);

    /// @brief Adds a method as a regular function for the given module
    /// @remark Be careful, this requires the given instance to be available as long as this function is callable from python
    template<typename TClass, typename TReturn, typename ... TArgs>
    void def(const Module& m, const char* name, TClass* instance, TReturn (TClass::*fn)(TArgs...), const char* doc = nullptr);

    /// @brief Adds a function accepting a varargs-tuple as argument to the given module
    template<typename TReturn>
    void def_varargs(const Module& m, const char* name, TReturn (*fn)(const Tuple&), const char* doc = nullptr);

    /// @brief Adds a function accepting a varargs-tuple and a keywords-dict as arguments to the given module
    template<typename TReturn>
    void def_varargs_keywords(const Module& m, const char* name, TReturn(*fn)(const Tuple&, const Dict&), const char* doc = nullptr);

    /// @brief Adds the specified class to the given module
    void def(const Module& m, const char* name, PythonClass* cls);

    /// @brief Adds the specified object to the given module
    template<typename T>
    void def(const Module& m, const char* name, const T& obj);

}

#include "Module.inl"

#endif // __PYTHON_MODULE_H__
