#include "Common.h"

#include "Python/Convert.h"

#include <QString>

namespace python
{
    template<>
    CORE_API PyObject* to_python(const QString& val)
    {
        return to_python(val.toUtf8().constData());
    }
    template<>
    CORE_API QString from_python(PyObject* obj)
    {
        return QString::fromUtf8(PyUnicode_AsUTF8(obj));
    }
}
