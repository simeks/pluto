#ifndef __CORE_PYTHON_MODULE_H__
#define __CORE_PYTHON_MODULE_H__

#include "PythonCommon.h"

#define MODULE_ADD_PYTHON_FUNCTION(TClass, Name, Doc) \
    add_function(#Name, (PyCFunction)PYTHON_FUNCTION_NAME_CLASS(TClass, Name), METH_VARARGS, Doc);


class PythonFunction;
class PythonClass;
class CORE_API PythonModule
{
public:
    explicit PythonModule(PyObject* module);
    explicit PythonModule(const char* name);
    virtual ~PythonModule();

    /// Can only be called after module has been created
    void add_function(const char *name, PyCFunction fn, int flags, const char *doc = NULL);
    
    /// Can only be called after module has been created
    void add_object(const char* name, PyObject* object);
    void add_object(const char* name, Object* object);

    /// Can only be called after module has been created
    void add_type(const char* name, PythonClass* type);

    PyObject* object(const char* name) const;

    template<typename T>
    T* object(const char* name) const;

    virtual void init_module();

    const char* name() const;

    PyObject* module();
    PyObject* dict();

private:
    PyModuleDef _module_def;

    PyObject* _module;

    std::vector<PythonFunction*> _functions;

};


template <class T, typename std::enable_if<std::is_base_of<PythonModule, T>::value, PythonModule>::type* = nullptr>
T* pyobject_extract_instance(PyObject* self)
{
    return (T*)PyCapsule_GetPointer(self, nullptr);
}

template<typename T>
T* PythonModule::object(const char* name) const
{
    return python_convert::from_python<T*>(object(name));
}

#endif // __CORE_PYTHON_MODULE_H__
