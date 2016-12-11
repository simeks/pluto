#ifndef __CORE_PYTHON_TYPE_H__
#define __CORE_PYTHON_TYPE_H__

#include "PythonCommon.h"

class CORE_API PythonType
{
public:
    explicit PythonType(const char* name, size_t basic_size);
    ~PythonType();

    void add_method(const char *name, PyCFunction meth, int flags, const char *doc = NULL);

    void set_new(newfunc fn);
    void set_init(initproc fn);
    void set_dealloc(destructor fn);

    void set_base(PythonType* base);

    int ready();

    PyObject* create_object(PyObject* args, PyObject* kw);

    bool check_type(PyObject* instance);

    const char* name() const;
    PyTypeObject* type_object();

    static void ready_all();

private:
    PyTypeObject _type;

    std::vector<PyMethodDef> _methods;

    static PythonType* _head;
    PythonType* _next;
};

#endif // __CORE_PYTHON_TYPE_H__
