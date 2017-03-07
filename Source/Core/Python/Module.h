#ifndef __PYTHON_MODULE_H__
#define __PYTHON_MODULE_H__

#include <Core/Python/Function.h>
#include <Core/Python/Object.h>

#define PYTHON_MODULE(Name) \
    void init_module_##Name##(python::Module m); \
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
            init_module_##Name##(python::Module(python::Borrowed(m))); \
        return m; \
    } \
    void init_module_##Name##(python::Module module)

#define PYTHON_MODULE_INSTALL(name) \
    PyImport_AppendInittab(#name, ::PyInit_##name##);


namespace python
{
    class CORE_API Module : public Object
    {
    public:
        Module(PyObject* obj);
        /// Constructor for borrowed references, this will increase the ref count of obj
        Module(Borrowed obj);

        /// @brief Adds an object to this module
        template<typename T>
        void def(const char* name, const T& obj);

        /// @brief Adds a regular function the the given module or class
        template<typename TReturn, typename ... TArgs>
        void def(const char* name, TReturn(*fn)(TArgs...), const char* doc = nullptr);

        /// @brief Adds a method as a regular function for the given module or class
        /// @remark Be careful, this requires the given instance to be available as long as this function is callable from python
        template<typename TClass, typename TReturn, typename ... TArgs>
        void def(const char* name, TClass* instance, TReturn(TClass::*fn)(TArgs...), const char* doc = nullptr);

        /// @brief Adds a function accepting a varargs-tuple as argument to the given module or class
        template<typename TReturn>
        void def_varargs(const char* name, TReturn(*fn)(const Tuple&), const char* doc = nullptr);

        /// @brief Adds a function accepting a varargs-tuple and a keywords-dict as arguments to the given module or class
        template<typename TReturn>
        void def_varargs_keywords(const char* name, TReturn(*fn)(const Tuple&, const Dict&), const char* doc = nullptr);

        /// @brief Adds the specified class to the given object
        /// @deprecated
        void def(const char* name, PythonClass* cls);

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
