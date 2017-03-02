#ifndef __PYTHON_SEQUENCE_H__
#define __PYTHON_SEQUENCE_H__

#include <Core/Python/Object.h>

namespace python
{
    /// Wrapper for python sequence
    class CORE_API Sequence : public python::Object
    {
    public:
        Sequence();
        explicit Sequence(const python::Object& s);
        ~Sequence();

        size_t size() const;

        /// Returns a borrowed reference
        python::Object get(size_t idx) const;

        bool valid() const;

        template<typename T>
        T get(size_t idx) const
        {
            return python::from_python<T>(get(idx));
        }
    };
}



#endif // __PYTHON_SEQUENCE_H__
