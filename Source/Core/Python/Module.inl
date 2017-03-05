namespace python
{
    template<typename T>
    INLINE void Module::def(const char* name, const T& obj)
    {
        python::def(*this, name, python::to_python(obj));
    }
    template<typename TReturn, typename ... TArgs>
    INLINE void Module::def(const char* name, TReturn(*fn)(TArgs...), const char* doc)
    {
        python::def(*this, name, fn, doc);
    }
    template<typename TClass, typename TReturn, typename ... TArgs>
    INLINE void Module::def(const char* name, TClass* instance, TReturn(TClass::*fn)(TArgs...), const char* doc)
    {
        python::def(*this, name, instance, fn, doc);
    }
    template<typename TReturn>
    INLINE void Module::def_varargs(const char* name, TReturn(*fn)(const Tuple&), const char* doc)
    {
        python::def_varargs(*this, name, fn, doc);
    }
    template<typename TReturn>
    INLINE void Module::def_varargs_keywords(const char* name, TReturn(*fn)(const Tuple&, const Dict&), const char* doc)
    {
        python::def_varargs_keywords(*this, name, fn, doc);
    }
}
