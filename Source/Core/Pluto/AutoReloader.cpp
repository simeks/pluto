#include <Core/Common.h>

#include "AutoReloader.h"
#include "Python/PythonCommon.h"

#include <QFileSystemWatcher>

AutoReloader::AutoReloader()
{
    _watcher = new QFileSystemWatcher();
    connect(_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(file_changed(const QString&)));
}
AutoReloader::~AutoReloader()
{
    delete _watcher;
}
void AutoReloader::add_module(PyObject* module)
{
    if (PyObject_HasAttrString(module, "__file__"))
    {
        QString file = PyUnicode_AsUTF8(PyObject_GetAttrString(module, "__file__"));

        auto it = _modules.find(file);
        if (it == _modules.end())
        {
            _modules[file] = module;
            _watcher->addPath(file);
        }
    }

}
void AutoReloader::file_changed(const QString& path)
{
    auto it = _modules.find(path);
    assert(it != _modules.end());
    if (!PyImport_ReloadModule(it->second))
        PyErr_Print();
}
