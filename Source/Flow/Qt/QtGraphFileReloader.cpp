#include <Core/Common.h>
#include <Core/Pluto/PlutoCore.h>

#include "FlowPythonModule.h"
#include "QtGraphFileReloader.h"

#include <QFileSystemWatcher>

GraphFileReloader::GraphFileReloader(FlowPythonModule* module) : _module(module)
{
    moveToThread(PlutoCore::instance().kernel_thread());

    _watcher = new QFileSystemWatcher();
    connect(_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(file_changed(const QString&)));
}
GraphFileReloader::~GraphFileReloader()
{
    delete _watcher;
}
void GraphFileReloader::add_file(const char* class_name, const char* file)
{
    _files[file] = class_name;
    _watcher->addPath(file);
}
void GraphFileReloader::file_changed(const QString& path)
{
    auto it = _files.find(path);
    if (it != _files.end())
        _module->install_graph_node_from_file(it->second.toUtf8().constData(), it->first.toUtf8().constData());
}

