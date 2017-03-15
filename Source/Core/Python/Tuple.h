#ifndef __PYTHON_TUPLE_H__
#define __PYTHON_TUPLE_H__

#include <Core/Python/Object.h>

#include <array>

namespace python
{
    /// Wrapper for python tuples
    class CORE_API Tuple : public python::Object
    {
    public:
        Tuple();
        explicit Tuple(size_t size);
        /// @remarks If given a bare PyObject* this will try to convert the object to a 
        ///          tuple if object already is a tuple the ref count is increased.
        explicit Tuple(const python::Object& t);

        size_t size() const;

        python::Object get(size_t idx) const;
        void set(size_t idx, const python::Object& obj);

        bool valid() const;

        template<typename T>
        T get(size_t idx) const
        {
            return python::from_python<T>(get(idx));
        }

        template<typename T>
        void set(size_t idx, T value)
        {
            set(idx, Object(to_python(value)));
        }
    };

	/// Converts all the given arguments to a tuple
	template<typename ... TArgs>
	Tuple make_tuple(TArgs... args);
}

#include "Tuple.inl"


#endif // __PYTHON_TUPLE_H__
