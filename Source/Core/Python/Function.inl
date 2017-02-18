
namespace python
{
    namespace function
    {
        template<typename TReturn, typename ... TArgs, typename TTuple, size_t... Index>
        PyObject* invoke(TReturn(*fn)(TArgs...), const TTuple& args, const std::index_sequence<Index...>&)
        {
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
            return python_convert::to_python<TReturn>((self->*fn)(std::get<Index>(args)...));
        }
        template<typename TClass, typename ... TArgs, typename TTuple, size_t... Index>
        PyObject* invoke(TClass* self, void(TClass::*fn)(TArgs...), const TTuple& args, const std::index_sequence<Index...>&)
        {
            (self->*fn)(std::get<Index>(args)...);
            Py_RETURN_NONE;
        }

        template<typename TReturn, typename ... TArgs>
        FunctionCaller<TReturn, TArgs...>::FunctionCaller(TReturn(*fn)(TArgs...)) : _fn(fn)
        {
        }

        template<typename TReturn, typename ... TArgs>
        PyObject* FunctionCaller<TReturn, TArgs...>::operator()(PyObject* args, PyObject* )
        {
            size_t i = 0;
            // Initializer lists are guaranteed to evaluate in order, therefore we can use them to first unpack all args
            std::tuple<typename std::decay<TArgs>::type...> t{
                python_convert::from_python<typename std::decay<TArgs>::type>(PyTuple_GetItem(args, i++))...
            };
            i; args; // Avoid C4189 when having no args
            // To unpack all args to actual function arguments we use a wrapper function which indexes all args and then unpacks them using ...-notation
            return invoke(_fn, t, std::index_sequence_for<TArgs...>{});
        }

        template<typename TClass, typename TReturn, typename ... TArgs>
        MethodCaller<TClass, TReturn, TArgs...>::MethodCaller(TClass* self, TReturn(TClass::*fn)(TArgs...)) : _self(self), _fn(fn)
        {
        }

        template<typename TClass, typename TReturn, typename ... TArgs>
        PyObject* MethodCaller<TClass, TReturn, TArgs...>::operator()(PyObject* args, PyObject* )
        {
            size_t i = 0;
            std::tuple<typename std::decay<TArgs>::type...> t{
                python_convert::from_python<typename std::decay<TArgs>::type>(PyTuple_GetItem(args, i++))...
            };
            return invoke(_self, _fn, t, std::index_sequence_for<TArgs...>{});
        }

        template<typename TReturn, typename ... TArgs>
        std::unique_ptr<CallerBase> make_caller(TReturn(*fn)(TArgs...))
        {
            return std::make_unique<FunctionCaller<TReturn, TArgs ...>>(fn);
        }

        template<typename TClass, typename TReturn, typename ... TArgs>
        std::unique_ptr<CallerBase> make_caller(TClass* self, TReturn(TClass::*fn)(TArgs...))
        {
            return std::make_unique<MethodCaller<TClass, TReturn, TArgs ...>>(self, fn);
        }
    }


    template<typename Fn>
    Object make_function(Fn* fn)
    {
        return make_function(function::make_caller(fn));
    }
}