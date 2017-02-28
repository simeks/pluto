#include "Common.h"

#include "Python/Convert.h"

#include <QString>

namespace
{
    struct QStringConverter
    {
        static PyObject* to_python(const QString& val)
        {
            return python::to_python(val.toUtf8().constData());
        }
        static QString from_python(PyObject* obj)
        {
            return QString::fromUtf8(PyUnicode_AsUTF8(obj));
        }
    };

    python::TypeConverter<QString, QStringConverter> qstring_converter;
}
