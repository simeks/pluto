
namespace python
{
    namespace function
    {
        template<typename TReturn, typename ... TArgs, typename TTuple, size_t... Index>
        PyObject* invoke(TReturn(*fn)(TArgs...), const TTuple& args, const std::index_sequence<Index...>&)
        {
            args; // Avoid C4189 when having no args
            return python_convert::to_python<TReturn>(fn(std::get<Index>(args)...));
        }
        template<typename ... TArgs, typename TTuple, size_t... Index>
        PyObject* invoke(void(*fn)(TArgs...), const TTuple& args, const std::index_sequence<Index...>&)
        {
            args; // Avoid C4189 when having no args
            fn(std::get<Index>(args)...);
            Py_RETURN_NONE;
        }

        template<typename TClass, typename TReturn, typename ... TArgs, typename TTuple, size_t... Index>
        PyObject* invoke(TClass* self, TReturn(TClass::*fn)(TArgs...), const TTuple& args, const std::index_sequence<Index...>&)
        {
            args; // Avoid C4189 when having no args
            return python_convert::to_python<TReturn>((self->*fn)(std::get<Index>(args)...));
        }
        template<typename TClass, typename ... TArgs, typename TTuple, size_t... Index>
        PyObject* invoke(TClass* self, void(TClass::*fn)(TArgs...), const TTuple& args, const std::index_sequence<Index...>&)
        {
            args; // Avoid C4189 when having no args
            (self->*fn)(std::get<Index>(args)...);
            Py_RETURN_NONE;
        }

        PyObject* DefaultArgumentPolicy::pop_item_from_tuple(PyObject* t, size_t& i)
        {
            return PyTuple_GetItem(t, i++);
        }

        template<typename ... TArgs>
        std::tuple<typename std::decay<TArgs>::type...> DefaultArgumentPolicy::unpack_args(PyObject* args, PyObject*)
        {
            if (PyTuple_Check(args) && PyTuple_Size(args) != sizeof...(TArgs)) 
            {
                PYTHON_ERROR_R(TypeError, 
                    std::tuple<typename std::decay<TArgs>::type...>(), 
                    "TypeError: function takes %d positional argument but %d were given", sizeof...(TArgs), PyTuple_Size(args));
            }

            size_t i = 0;
            // Initializer lists are guaranteed to evaluate in order, therefore we can use them to first unpack all args
            std::tuple<typename std::decay<TArgs>::type...> t{
                python_convert::from_python<typename std::decay<TArgs>::type>(pop_item_from_tuple(args, i))...
            };
            i; args; // Avoid C4189 when having no args

            return t;
        }
        template<typename ... TArgs>
        std::tuple<Tuple> VarargsArgumentPolicy::unpack_args(PyObject* args, PyObject*)
        {
            return {Tuple(args)};
        }
        template<typename ... TArgs>
        std::tuple<Tuple, Dict> VarargsKeywordsArgumentPolicy::unpack_args(PyObject* args, PyObject* kw)
        {
            return {Tuple(args), Dict(kw)};
        }

        template<typename TArgPolicy, typename TReturn, typename ... TArgs>
        FunctionCaller<TArgPolicy, TReturn, TArgs...>::FunctionCaller(TReturn(*fn)(TArgs...)) : _fn(fn)
        {
        }

        template<typename TArgPolicy, typename TReturn, typename ... TArgs>
        PyObject* FunctionCaller<TArgPolicy, TReturn, TArgs...>::operator()(PyObject* args, PyObject* kw)
        {
            auto t = TArgPolicy::unpack_args<TArgs...>(args, kw);
            if (PyErr_Occurred())
                return 0;
            // To unpack all args to actual function arguments we use a wrapper function which indexes all args and then unpacks them using ...-notation
            return invoke(_fn, t, std::index_sequence_for<TArgs...>{});
        }

        template<typename TArgPolicy, typename TClass, typename TReturn, typename ... TArgs>
        MethodCaller<TArgPolicy, TClass, TReturn, TArgs...>::MethodCaller(TClass* self, TReturn(TClass::*fn)(TArgs...)) : _self(self), _fn(fn)
        {
        }

        template<typename TArgPolicy, typename TClass, typename TReturn, typename ... TArgs>
        PyObject* MethodCaller<TArgPolicy, TClass, TReturn, TArgs...>::operator()(PyObject* args, PyObject* kw)
        {
            auto t = TArgPolicy::unpack_args<TArgs...>(args, kw);
            if (PyErr_Occurred())
                return 0;
            return invoke(_self, _fn, t, std::index_sequence_for<TArgs...>{});
        }

        template<typename TArgPolicy, typename TReturn, typename ... TArgs>
        std::unique_ptr<CallerBase> make_caller(TReturn(*fn)(TArgs...))
        {
            return std::make_unique<FunctionCaller<TArgPolicy, TReturn, TArgs ...>>(fn);
        }

        template<typename TArgPolicy, typename TClass, typename TReturn, typename ... TArgs>
        std::unique_ptr<CallerBase> make_caller(TClass* self, TReturn(TClass::*fn)(TArgs...))
        {
            return std::make_unique<MethodCaller<TArgPolicy, TClass, TReturn, TArgs ...>>(self, fn);
        }
    }

    template<typename Fn>
    Object make_function(Fn* fn, const char* name, const char* doc)
    {
        return make_function(function::make_caller<function::DefaultArgumentPolicy>(fn), name, doc);
    }

    template<typename TClass, typename TReturn, typename ... TArgs>
    Object make_function(TClass* self, TReturn(TClass::*fn)(TArgs...), const char* name, const char* doc)
    {
        return make_function(function::make_caller<function::DefaultArgumentPolicy>(self, fn), name, doc);
    }

    template<typename TReturn>
    Object make_varargs_function(TReturn(*fn)(const Tuple&), const char* name, const char* doc)
    {
        return make_function(function::make_caller<function::VarargsArgumentPolicy>(fn), name, doc);
    }

    template<typename TClass, typename TReturn>
    Object make_varargs_function(TClass* self, TReturn(TClass::*fn)(const Tuple&), const char* name, const char* doc)
    {
        return make_function(function::make_caller<function::VarargsArgumentPolicy>(self, fn), name, doc);
    }

    template<typename TReturn>
    Object make_varargs_keywords_function(TReturn(*fn)(const Tuple&, const Dict&), const char* name, const char* doc)
    {
        return make_function(function::make_caller<function::VarargsKeywordsArgumentPolicy>(fn), name, doc);
    }

    template<typename TClass, typename TReturn>
    Object make_varargs_keywords_function(TClass* self, TReturn(TClass::*fn)(const Tuple&, const Dict&), const char* name, const char* doc)
    {
        return make_function(function::make_caller<function::VarargsKeywordsArgumentPolicy>(self, fn), name, doc);
    }
}