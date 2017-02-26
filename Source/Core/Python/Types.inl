namespace python
{
    namespace type_registry
    {
        INLINE Entry::Entry(const std::type_info& cpp_type) :
            cpp_type(cpp_type),
            py_type(nullptr),
            to_python(nullptr),
            from_python(nullptr)
        {
        }
    }

    template<typename T>
    const type_registry::Entry& TypeInfo<T>::info = type_registry::lookup(typeid(T));


}