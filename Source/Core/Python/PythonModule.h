#ifndef __CORE_PYTHON_MODULE_H__
#define __CORE_PYTHON_MODULE_H__

#include "PythonCommon.h"

#define MODULE_ADD_PYTHON_FUNCTION(TClass, Name, Flags, Doc) \
    add_function(#Name, (PyCFunction)PYTHON_FUNCTION_NAME_CLASS(TClass, Name), Flags, Doc);


class PythonFunction;
class PythonType;
class PythonModule
{
public:
    explicit PythonModule(PyObject* module);
    explicit PythonModule(const char* name);
    virtual ~PythonModule();

    /// Can only be called after module has been created
    void add_function(const char *name, PyCFunction fn, int flags, const char *doc = NULL);
    
    /// Can only be called after module has been created
    void add_object(const char* name, PyObject* object);

    /// Can only be called after module has been created
    void add_type(const char* name, PythonType* type);

    virtual void create();

    const char* name() const;

    PyObject* module();
    PyObject* dict();

private:
    PyModuleDef _module_def;

    PyObject* _module;

    std::vector<PythonFunction*> _functions;

};


#endif // __CORE_PYTHON_MODULE_H__
