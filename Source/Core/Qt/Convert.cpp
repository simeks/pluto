#include "Common.h"

#include "Python/Convert.h"

#include <QString>

namespace python_convert
{
    template<>
    CORE_API PyObject* to_python<QString>(const QString& val)
    {
        return to_python(val.toUtf8().constData());
    }
    template<>
    CORE_API QString from_python<QString>(PyObject* obj)
    {
        return QString(PyUnicode_AsUTF8(obj));
    }
}
