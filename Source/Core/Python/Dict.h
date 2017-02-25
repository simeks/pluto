#ifndef __PYTHON_DICT_H__
#define __PYTHON_DICT_H__

#include <Core/Python/Object.h>

namespace python
{
    /// Wrapper for python dicts
    class CORE_API Dict : public Object
    {
    public:
        Dict();
        explicit Dict(const Object& d);

        bool has_key(const char* key) const;

        Object get(const char* key) const;

        void set(const char* key, const Object& item);
        void clear();

        int next(size_t* pos, PyObject** key, PyObject** value) const;

        bool valid() const;

        Dict copy() const;

        template<typename T>
        T get(const char* key) const
        {
            return from_python<T>(get(key));
        }

        template<typename T>
        void set(const char* key, T value)
        {
            set(key, to_python(value));
        }

    };
}

#endif // __PYTHON_DICT_H__
