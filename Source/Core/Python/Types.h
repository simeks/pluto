#ifndef __PYTHON_TYPES_H__
#define __PYTHON_TYPES_H__

#include "Object.h"

#include <typeindex>

namespace python
{
    typedef PyObject* (*ToPythonFunction)(void const*);
    typedef void(*FromPythonFunction)(PyObject*, void*);

    namespace type_registry
    {
        struct Entry
        {
            Entry(const std::type_info& cpp_type);

            /// Type info aquired by using typeid()
            const std::type_index cpp_type;

            /// Converters

            ToPythonFunction to_python;
            FromPythonFunction from_python;

            /// Optional: Mainly used for new types
            PyTypeObject* py_type;
        };

        CORE_API const Entry& lookup(const std::type_info& type);

        CORE_API void insert(const std::type_info& type,
                             ToPythonFunction to_fn, 
                             FromPythonFunction from_fn);

        CORE_API void insert(const std::type_info& type,
                             PyTypeObject* py_type,
                             ToPythonFunction to_fn,
                             FromPythonFunction from_fn);
    }

    template<typename T>
    struct TypeInfo
    {
        static const type_registry::Entry& info;
    };

}

#include "Types.inl"

#endif // __PYTHON_TYPES_H__
