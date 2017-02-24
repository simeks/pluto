#ifndef __PYTHON_DICT_H__
#define __PYTHON_DICT_H__

#include <Core/Python/Object.h>

namespace python
{
    /// Wrapper for python dicts
    class CORE_API Dict : public python::Object
    {
    public:
        Dict();
        explicit Dict(const python::Object& d);

        bool has_key(const char* key) const;

        python::Object get(const char* key) const;

        void set(const char* key, const python::Object& item);
        void clear();

        int next(size_t* pos, PyObject** key, PyObject** value) const;

        bool valid() const;

        Dict copy() const;

        template<typename T>
        T get(const char* key) const
        {
            return python::from_python<T>(get(key));
        }

        template<typename T>
        void set(const char* key, T value)
        {
            set(key, python::to_python(value));
        }

    };
}

#endif // __PYTHON_DICT_H__
