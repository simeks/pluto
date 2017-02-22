#ifndef __CORE_PYTHON_HELPERS_H__
#define __CORE_PYTHON_HELPERS_H__

#include "Convert.h"
#include "PythonWrapper.h"

#define _PYTHON_PARSE_ARGS_ERROR_CHECK(N) \
    if (PySequence_Length(args) != N) \
    { \
        PyErr_SetString(PyExc_TypeError, "wrong number of arguments"); \
        return; \
    } \
    PyObject* obj[N]; \
    for (int i = 0; i < N; ++i) \
    { \
        obj[i] = PySequence_Fast_GET_ITEM(args, i); \
        if (!obj[i]) \
        { \
            PyErr_SetString(PyExc_TypeError, "missing required argument"); \
            return; \
        } \
    }

namespace python_helpers
{
    template<typename A>
    void parse_args(PyObject* args, A& a)
    {
        if (PySequence_Check(args))
        {
            _PYTHON_PARSE_ARGS_ERROR_CHECK(1);
            a = python::from_python<A>(obj[0]);
        }
    }
    template<typename A, typename B>
    void parse_args(PyObject* args, A& a, B& b)
    {
        if (PySequence_Check(args))
        {
            _PYTHON_PARSE_ARGS_ERROR_CHECK(2);
            a = python::from_python<A>(obj[0]);
            b = python::from_python<B>(obj[1]);
        }
    }
    template<typename A, typename B, typename C>
    void parse_args(PyObject* args, A& a, B& b, C& c)
    {
        if (PySequence_Check(args))
        {
            _PYTHON_PARSE_ARGS_ERROR_CHECK(3);
            a = python::from_python<A>(obj[0]);
            b = python::from_python<B>(obj[1]);
            c = python::from_python<C>(obj[2]);
        }
    }
    template<typename A, typename B, typename C, typename D>
    void parse_args(PyObject* args, A& a, B& b, C& c, D& d)
    {
        if (PySequence_Check(args))
        {
            _PYTHON_PARSE_ARGS_ERROR_CHECK(4);
            a = python::from_python<A>(obj[0]);
            b = python::from_python<B>(obj[1]);
            c = python::from_python<C>(obj[2]);
            d = python::from_python<D>(obj[3]);
        }
    }
    template<typename A>
    PyObject* build_args(const A& a)
    {
        PyObject* t = PyTuple_New(1);
        PyTuple_SetItem(t, 0, python::to_python(a).ptr());
        return t;
    }
    template<typename A, typename B>
    PyObject* build_args(const A& a, const B& b)
    {
        PyObject* t = PyTuple_New(2);
        PyTuple_SetItem(t, 0, python::to_python(a).ptr());
        PyTuple_SetItem(t, 1, python::to_python(b).ptr());
        return t;
    }
    template<typename A, typename B, typename C>
    PyObject* build_args(const A& a, const B& b, const C& c)
    {
        PyObject* t = PyTuple_New(2);
        PyTuple_SetItem(t, 0, python::to_python(a).ptr());
        PyTuple_SetItem(t, 1, python::to_python(b).ptr());
        PyTuple_SetItem(t, 2, python::to_python(c).ptr());
        return t;
    }
}

#endif // __CORE_PYTHON_HELPERS_H__
