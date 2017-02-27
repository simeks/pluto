namespace python
{
    template<typename T>
    T from_python(PyObject* obj)
    {
        FromPythonFunction conv = TypeInfo<T>::info.from_python;
        if (!conv)
        {
            PyErr_Format(PyExc_TypeError, "from_python: No converter found for type %s", 
                TypeInfo<T>::info.cpp_type.name());
            return T();
        }
        T val; // TODO: This will call the constructor for non primitives object, can this be avoided?
        conv(obj, &val);
        return val;
    }

    template<typename T>
    T from_python(const Object& obj)
    {
        return from_python<T>(obj.ptr());
    }

    template<typename T>
    PyObject* to_python(const T& value)
    {
        ToPythonFunction conv = TypeInfo<T>::info.to_python;
        if (!conv)
        {
            PyErr_Format(PyExc_TypeError, "to_python: No converter found for type %s",
                TypeInfo<T>::info.cpp_type.name());
            Py_RETURN_NONE;
        }
        return conv(&value);
    }


    template<typename T, typename Converter>
    PyObject* to_python_wrapper(void const* val)
    {
        return Converter::to_python(*((T*)val));
    }

    template<typename T, typename Converter>
    void from_python_wrapper(PyObject* obj, void* val)
    {
        new (val) T(Converter::from_python(obj));
    }

    template<typename T, typename Converter>
    TypeConverter<T, Converter>::TypeConverter()
    {
        type_registry::insert(typeid(T),
            to_python_wrapper<T, Converter>,
            from_python_wrapper<T, Converter>);
    }

}

