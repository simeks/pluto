#ifndef __PYTHON_STR_H__
#define __PYTHON_STR_H__

#include <Core/Python/Object.h>

namespace python
{
    /// Wrapper for python strings
    class CORE_API Str : public python::Object
    {
    public:
        Str();
        Str(const char* str);
        /// @remarks If given a bare PyObject* this will try to convert the object to a 
        ///          string if object already is a string the ref count is increased.
        explicit Str(const python::Object& str);

    };

}

#endif // __PYTHON_STR_H__
