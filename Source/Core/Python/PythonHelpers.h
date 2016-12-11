#ifndef __CORE_PYTHON_HELPERS_H__
#define __CORE_PYTHON_HELPERS_H__

#include "Convert.h"
#include "PythonWrapper.h"

namespace python_helpers
{
    template<typename A>
    void parse_args(PyObject* args, A& a)
    {
        if (PySequence_Check(args))
        {
            a = python_convert::from_python<A>(PySequence_GetItem(args, 0));
        }
    }
    template<typename A, typename B>
    void parse_args(PyObject* args, A& a, B& b)
    {
        if (PySequence_Check(args))
        {
            a = python_convert::from_python<A>(PySequence_GetItem(args, 0));
            b = python_convert::from_python<B>(PySequence_GetItem(args, 1));
        }
    }
    template<typename A, typename B, typename C>
    void parse_args(PyObject* args, A& a, B& b, C& c)
    {
        if (PySequence_Check(args))
        {
            a = python_convert::from_python<A>(PySequence_GetItem(args, 0));
            b = python_convert::from_python<B>(PySequence_GetItem(args, 1));
            c = python_convert::from_python<C>(PySequence_GetItem(args, 2));
        }
    }
    template<typename A, typename B, typename C, typename D>
    void parse_args(PyObject* args, A& a, B& b, C& c, D& d)
    {
        if (PySequence_Check(args))
        {
            a = python_convert::from_python<A>(PySequence_GetItem(args, 0));
            b = python_convert::from_python<B>(PySequence_GetItem(args, 1));
            c = python_convert::from_python<C>(PySequence_GetItem(args, 2));
            d = python_convert::from_python<D>(PySequence_GetItem(args, 3));
        }
    }
}

#endif // __CORE_PYTHON_HELPERS_H__
