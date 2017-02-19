namespace python
{
    template<typename TReturn, typename ... TArgs>
    INLINE void def(const Module& m, const char* name, TReturn(*fn)(TArgs...), const char* doc)
    {
        python::setattr(m, name, python::make_function(fn, name, doc));
    }

    template<typename TClass, typename TReturn, typename ... TArgs>
    INLINE void def(const Module& m, const char* name, TClass* self, TReturn(TClass::*fn)(TArgs...), const char* doc)
    {
        python::setattr(m, name, python::make_function(self, fn, name, doc));
    }

    template<typename TReturn>
    INLINE void def_varargs(const Module& m, const char* name, TReturn(*fn)(const Tuple&), const char* doc)
    {
        python::setattr(m, name, python::make_varargs_function(fn, name, doc));
    }

    template<typename TClass, typename TReturn>
    INLINE void def_varargs(const Module& m, const char* name, TClass* self, TReturn(TClass::*fn)(const Tuple&), const char* doc)
    {
        python::setattr(m, name, python::make_varargs_function(self, fn, name, doc));
    }

    template<typename TReturn>
    INLINE void def_varargs_keywords(const Module& m, const char* name, TReturn(*fn)(const Tuple&, const Dict&), const char* doc)
    {
        python::setattr(m, name, python::make_varargs_keywords_function(fn, name, doc));
    }

    template<typename TClass, typename TReturn>
    INLINE void def_varargs_keywords(const Module& m, const char* name, TClass* self, TReturn(TClass::*fn)(const Tuple&, const Dict&), const char* doc)
    {
        python::setattr(m, name, python::make_varargs_keywords_function(self, fn, name, doc));
    }

    INLINE void def(const Module& m, const char* name, PythonClass* cls)
    {
        python::setattr(m, name, (PyObject*)cls->python_type());
    }
    
    template<typename T>
    INLINE void def(const Module& m, const char* name, const T& obj)
    {
        python::setattr(m, name, python_convert::to_python(obj));
    }

}