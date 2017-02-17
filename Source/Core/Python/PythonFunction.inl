
namespace python
{
    namespace function
    {
        template<typename TFunction, typename TTuple, size_t... Index>
        void invoke(TFunction fn, const TTuple& args, const std::index_sequence<Index...>&)
        {
            fn(std::get<Index>(args)...);
        }
        template<typename TClass, typename TFunction, typename TTuple, size_t... Index>
        void invoke(TClass* self, TFunction fn, const TTuple& args, const std::index_sequence<Index...>&)
        {
            (self->*fn)(std::get<Index>(args)...);
        }

        template<typename ReturnT, typename ... Args>
        FunctionCaller::FunctionCaller(TReturn(*fn)(TArgs...)) : _fn(fn)
        {
        }

        template<typename TReturn, typename ... TArgs>
        PyObject* FunctionCaller::operator()(PyObject* args, PyObject* kw)
        {
            int i = 0;
            // Initializer lists are guaranteed to evaluate in order, therefore we can use them to first unpack all args
            std::tuple<Args...> args{ convert<Args>(PyTuple_GetItem(args, i++))... };
            // To unpack all args to actual function arguments we use a wrapper function which indexes all args and then unpacks them using ...-notation
            invoke(_fn, args, std::index_sequence_for<Args...>{});
        }

        template<typename TClass, typename ReturnT, typename ... Args>
        MethodCaller::MethodCaller(TClass* self, TReturn(TClass::*fn)(TArgs...)) : _self(self), _fn(fn)
        {
        }

        template<typename TClass, typename ReturnT, typename ... Args>
        PyObject* MethodCaller::operator()(PyObject* args, PyObject* kw)
        {
            int i = 0;
            std::tuple<Args...> args{ convert<Args>(PyTuple_GetItem(args, i++))... };
            invoke(_self, _fn, args, std::index_sequence_for<Args...>{});
        }

        template<typename TReturn, typename ... TArgs>
        std::unique_ptr<CallerBase> make_caller(TReturn(*meth)(TArgs...))
        {
            return std::make_unique<Caller<TReturn, TArgs ...>();
        }

        template<typename TClass, typename TReturn, typename ... TArgs>
        std::unique_ptr<CallerBase> make_caller(TClass* self, TReturn(TClass::*meth)(TArgs...))
        {
            return std::make_unique<Caller<TClass, TReturn, TArgs ...>();
        }
    }
}