#ifndef __PYTHON_FUNCTION_H__
#define __PYTHON_FUNCTION_H__

#include "Object.h"

namespace python
{
    namespace function
    {
        struct CallerBase
        {
            virtual PyObject* operator()(PyObject* args, PyObject* kw) = 0;
        };

        template<typename TReturn, typename ... TArgs>
        struct FunctionCaller : CallerBase
        {
            TReturn(*_fn)(TArgs...);

            FunctionCaller(TReturn(*fn)(TArgs...));

            PyObject* operator()(PyObject* args, PyObject* kw);
        };

        template<typename TClass, typename TReturn, typename ... TArgs>
        struct MethodCaller : CallerBase
        {
            TClass* _self;
            TReturn(TClass::*_fn)(TArgs...);

            MethodCaller(TClass* self, TReturn(TClass::*fn)(TArgs...));

            PyObject* operator()(PyObject* args, PyObject* kw);
        };

        template<typename TClass, typename TReturn, typename ... TArgs>
        std::unique_ptr<CallerBase> make_caller(TReturn(*fn)(TArgs...));

        template<typename TClass, typename TReturn, typename ... TArgs>
        std::unique_ptr<CallerBase> make_caller(TClass* self, TReturn(TClass::*fn)(TArgs...));
    }
}

namespace python
{
    CORE_API Object make_function(std::unique_ptr<function::CallerBase> caller);

    template<typename Fn>
    Object make_function(Fn* fn);
}


#include "Function.inl"



#endif // __PYTHON_FUNCTION_H__
