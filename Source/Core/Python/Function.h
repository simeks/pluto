#ifndef __PYTHON_FUNCTION_H__
#define __PYTHON_FUNCTION_H__

#include <Core/Python/PythonCommon.h>

namespace python
{
    namespace function
    {
        struct CallerBase
        {
            virtual PyObject* operator()(PyObject* args, PyObject* kw) = 0;
        };

        /// @brief Default argument policy, this unpacks all by popping them from the tuple we got from Python
        /// Performs error checking to find things such invalid number of arguments, etc
        struct DefaultArgumentPolicy
        {
            /// Returns the item at index i before incrementing i
            static PyObject* pop_item_from_tuple(PyObject* t, size_t& i);

            template<typename ... TArgs>
            static std::tuple<typename std::decay<TArgs>::type...> unpack_args(PyObject*, PyObject*);
        };

        /// @brief Policy for passing the varargs-tuple from python directly to the function
        struct VarargsArgumentPolicy
        {
            template<typename ... TArgs>
            static std::tuple<Tuple> unpack_args(PyObject*, PyObject*);
        };

        /// @brief Policy for passing the varargs-tuple and keywords-dict from python directly to the function
        struct VarargsKeywordsArgumentPolicy
        {
            template<typename ... TArgs>
            static std::tuple<Tuple, Dict> unpack_args(PyObject*, PyObject*);
        };

        template<typename TArgPolicy, typename TReturn, typename ... TArgs>
        struct FunctionCaller : CallerBase
        {
            TReturn(*_fn)(TArgs...);

            FunctionCaller(TReturn(*fn)(TArgs...));

            PyObject* operator()(PyObject* args, PyObject* kw);
        };

        template<typename TArgPolicy, typename TClass, typename TReturn, typename ... TArgs>
        struct MethodCaller : CallerBase
        {
            TClass* _self;
            TReturn(TClass::*_fn)(TArgs...);

            MethodCaller(TClass* self, TReturn(TClass::*fn)(TArgs...));

            PyObject* operator()(PyObject* args, PyObject* kw);
        };

        template<typename TArgPolicy, typename TClass, typename TReturn, typename ... TArgs>
        std::unique_ptr<CallerBase> make_caller(TReturn(*fn)(TArgs...));

        template<typename TArgPolicy, typename TClass, typename TReturn, typename ... TArgs>
        std::unique_ptr<CallerBase> make_caller(TClass* self, TReturn(TClass::*fn)(TArgs...));
    }
}

namespace python
{
    CORE_API Object make_function(
        std::unique_ptr<function::CallerBase> caller, const char* name, 
        const char* doc = nullptr);

    template<typename Fn>
    Object make_function(Fn* fn, const char* name, const char* doc = nullptr);

    template<typename TClass, typename TReturn, typename ... TArgs>
    Object make_function(TClass* self, TReturn (TClass::*fn)(TArgs...), const char* name, const char* doc = nullptr);

    template<typename TReturn>
    Object make_varargs_function(TReturn (*fn)(const Tuple&), const char* name, const char* doc = nullptr);

    template<typename TClass, typename TReturn>
    Object make_varargs_function(TClass* self, TReturn(TClass::*fn)(const Tuple&), const char* name, const char* doc = nullptr);

    template<typename TReturn>
    Object make_varargs_keywords_function(TReturn(*fn)(const Tuple&, const Dict&), const char* name, const char* doc = nullptr);

    template<typename TClass, typename TReturn>
    Object make_varargs_keywords_function(TClass* self, TReturn(TClass::*fn)(const Tuple&, const Dict&), const char* name, const char* doc = nullptr);

}


#include "Function.inl"



#endif // __PYTHON_FUNCTION_H__
