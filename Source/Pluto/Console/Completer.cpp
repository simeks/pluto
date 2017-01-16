#include <Core/Common.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Pluto/PlutoKernel.h>
#include <Core/Python/PythonCommon.h>
#include <Core/Python/PythonModule.h>

#include "Completer.h"

#include <QDir>
#include <QRegExp>
#include <QString>
#include <QStringList>

Completer::Completer()
{
}
Completer::~Completer()
{
}
QString Completer::complete(const QString& cmdline, int begin, int end, QStringList& out)
{
    QString cmd = cmdline.mid(begin, end - begin);
    std::cout << cmdline.toStdString() << ", " << cmd.toStdString() << std::endl;

    complete_python(cmd, out);
    complete_file(cmd, out);

    if (out.size())
    {
        QString common_prefix = out[0];
        for (const QString& c : out)
        {
            while (!c.startsWith(common_prefix) && common_prefix != "")
            {
                common_prefix.remove(common_prefix.length() - 1, 1);
            }
        }
        QString ret = cmdline.left(begin) + common_prefix + cmd.mid(end);
        return ret;
    }
    return cmdline;
}

void Completer::complete_python(const QString& cmd, QStringList& out)
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
void Completer::complete_file(const QString& cmd, QStringList& out)
{
    if (cmd.isEmpty())
        return;

    QString file = cmd;
    file.replace('\\', '/');
    if (cmd.contains("/"))
    {
        int i = file.lastIndexOf('/');
        {
            QString prefix = file.left(i+1);
            QString suffix = file.mid(i+1);
            
            QDir dir(prefix);
            for (auto& fi : dir.entryInfoList(QStringList() << suffix + "*", QDir::Dirs | QDir::Files | QDir::Drives | QDir::NoDotAndDotDot))
            {
                if (fi.isDir())
                {
                    out.push_back(prefix + fi.fileName() + '/');
                }
                else
                {
                    out.push_back(prefix + fi.fileName());
                }
            }
        }

    }
    else
    {
        QDir dir;
        for (auto& fi : dir.entryInfoList(QStringList() << cmd + "*", QDir::Dirs | QDir::Files | QDir::Drives | QDir::NoDotAndDotDot))
        {
            if (fi.isDir())
            {
                out.push_back(fi.fileName() + '/');
            }
            else
            {
                out.push_back(fi.fileName());
            }
        }
    }
}

