#include <Core/Common.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Pluto/PlutoKernel.h>
#include <Core/Python/PythonCommon.h>
#include <Core/Python/PythonModule.h>

#include "Completer.h"

#include <QRegExp>
#include <QString>
#include <QStringList>

Completer::Completer()
{
}
Completer::~Completer()
{
}
void Completer::complete(const QString& cmd, QStringList& out)
{
    if (cmd.isEmpty())
        return;

    Dict main_dict = PlutoCore::instance().kernel()->main_module()->object<Dict>("__dict__");
    Dict builtins = Dict(PyObject_GetAttrString(main_dict.get("__builtins__"), "__dict__"));

    if (cmd.contains("."))
    {
        // From here we have to consider what we have before the '.'. 
        // For this we try to evaluate what ever is before '.' and then use PyObject_Dir() on the object find any matching attributes

        QRegExp re("(\\S+(\\.\\w+)*)\\.(\\w*)$");
        int i = re.indexIn(cmd);
        if (i != -1)
        {
            QString expr = re.cap(1);
            QString attr = re.cap(3);

            PyObject* ret = PyRun_String(expr.toUtf8().constData(), Py_eval_input, main_dict.dict(), nullptr);
            if (ret)
            {
                PyObject* l = PyObject_Dir(ret);
                for (int li = 0; li < PyList_Size(l); ++li)
                {
                    QString qkey = PyUnicode_AsUTF8(PyList_GetItem(l, li));
                    if (qkey.startsWith("__")) // Skip __attributes__
                        continue;
                    if (qkey.startsWith(attr))
                        out.push_back(expr + "." + qkey);
                }
            }
        }
    }
    else
    {
        // Global autocomplete

        PyObject *key, *value;
        size_t pos = 0;
        while (main_dict.next(&pos, &key, &value))
        {
            QString qkey = PyUnicode_AsUTF8(key);
            if (qkey.startsWith(cmd))
                out.push_back(qkey);
        }
        pos = 0;
        while (builtins.next(&pos, &key, &value))
        {
            QString qkey = PyUnicode_AsUTF8(key);
            if (qkey.startsWith(cmd))
                out.push_back(qkey);
        }
    }
}

