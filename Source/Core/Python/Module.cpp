#include "Common.h"

#include "Module.h"
#include "PythonCommon.h"

namespace python
{
    Module::Module(PyObject* m) : Object(m)
    {
    }
}
