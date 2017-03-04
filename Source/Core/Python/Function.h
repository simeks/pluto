#ifndef __PYTHON_FUNCTION_H__
#define __PYTHON_FUNCTION_H__

#include <Core/Python/Convert.h>
#include <Core/Python/Dict.h>
#include <Core/Python/PythonWrapper.h>
#include <Core/Python/Tuple.h>

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
            template<typename TTuple>
            static std::tuple<Tuple> unpack_args(PyObject*, PyObject*);

            template<typename TSelf, typename TTuple>
            static std::tuple<TSelf, Tuple> unpack_args(PyObject*, PyObject*);
        };

        /// @brief Policy for passing the varargs-tuple and keywords-dict from python directly to the function
        struct VarargsKeywordsArgumentPolicy
        {
            template<typename TTuple, typename TDict>
            static std::tuple<Tuple, Dict> unpack_args(PyObject*, PyObject*);

            template<typename TSelf, typename TTuple, typename TDict>
            static std::tuple<TSelf, Tuple, Dict> unpack_args(PyObject*, PyObject*);
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

        /// Caller is responsible for deleting the returned pointer
        template<typename TArgPolicy, typename TClass, typename TReturn, typename ... TArgs>
        CallerBase* make_caller(TReturn(*fn)(TArgs...));

        /// Caller is responsible for deleting the returned pointer
        template<typename TArgPolicy, typename TClass, typename TReturn, typename ... TArgs>
        CallerBase* make_caller(TClass* self, TReturn(TClass::*fn)(TArgs...));
    }
}

namespace python
{
    /// @remark This function takes ownership of the caller object, deleting it whenever done with it.
    CORE_API Object make_function(
        function::CallerBase* caller, const char* name, 
        const char* doc = nullptr);

    template<typename Fn>
    Object make_function(Fn* fn, const char* name, const char* doc = nullptr);

    template<typename TClass, typename TReturn, typename ... TArgs>
    Object make_function(TClass* self, TReturn (TClass::*fn)(TArgs...), const char* name, const char* doc = nullptr);

    template<typename Fn>
    Object make_varargs_function(Fn fn, const char* name, const char* doc = nullptr);

    template<typename TClass, typename TReturn, typename ... TArgs>
    Object make_varargs_function(TClass* self, TReturn(TClass::*fn)(TArgs...), const char* name, const char* doc = nullptr);

    template<typename Fn>
    Object make_varargs_keywords_function(Fn fn, const char* name, const char* doc = nullptr);

    template<typename TClass, typename TReturn, typename ... TArgs>
    Object make_varargs_keywords_function(TClass* self, TReturn(TClass::*fn)(TArgs...), const char* name, const char* doc = nullptr);


    /// @brief Adds a regular function the the given module or class
    template<typename TReturn, typename ... TArgs>
    void def(const Object& m, const char* name, TReturn(*fn)(TArgs...), const char* doc = nullptr);

    /// @brief Adds a method as a regular function for the given module or class
    /// @remark Be careful, this requires the given instance to be available as long as this function is callable from python
    template<typename TClass, typename TReturn, typename ... TArgs>
    void def(const Object& m, const char* name, TClass* instance, TReturn(TClass::*fn)(TArgs...), const char* doc = nullptr);

    /// @brief Adds a function accepting a varargs-tuple as argument to the given module or class
    template<typename TReturn>
    void def_varargs(const Object& m, const char* name, TReturn(*fn)(const Tuple&), const char* doc = nullptr);

    /// @brief Adds a function accepting a varargs-tuple and a keywords-dict as arguments to the given module or class
    template<typename TReturn>
    void def_varargs_keywords(const Object& m, const char* name, TReturn(*fn)(const Tuple&, const Dict&), const char* doc = nullptr);

}


#include "Function.inl"



#endif // __PYTHON_FUNCTION_H__
