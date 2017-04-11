namespace python
{
	/// TODO: @hack This should be reconsidered, probably the whole conversion model
	/// When converting a value from python we want a temporary storage for the resulting object.
	///		We want to avoid calling the constructor of said object as we only want the object
	///		to be copy-constructed during conversion. At the same time we still need to call the
	///		destructor of the temporary variable.
	template<typename T>
	struct ConvertValue
	{
		ConvertValue() {}
		~ConvertValue() 
		{
			// Call destructor, assuming object has been constructed
			ptr()->~T();
		}

		T* ptr() { return (T*)&value; }

		uint8_t value[sizeof(T)];
	};

    template<typename T>
    T from_python(PyObject* obj)
    {
        FromPythonFunction conv = TypeInfo<T>::info.from_python;
        if (!conv)
        {
            PYTHON_ERROR(PyExc_TypeError, "from_python: No converter found for type %s", 
                TypeInfo<T>::info.cpp_type.name());
        }

		ConvertValue<T> val;
        conv(obj, val.ptr());
        return *val.ptr();
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
            PYTHON_ERROR(PyExc_TypeError, "to_python: No converter found for type %s",
                TypeInfo<T>::info.cpp_type.name());
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

