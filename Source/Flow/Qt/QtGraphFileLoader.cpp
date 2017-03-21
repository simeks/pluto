#include <Core/Common.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Python/PythonCommon.h>

#include "FlowModule.h"
#include "Qt/Style.h"
#include "QtGraphFileLoader.h"
#include "RunGraphNode.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFileSystemWatcher>

GraphFileLoader::GraphFileLoader(const char* path)
{
    _root = QDir(path).absolutePath();

    _watcher = new QFileSystemWatcher(this);
    connect(_watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(dir_changed(const QString&)));
    connect(_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(file_changed(const QString&)));
    _watcher->addPath(_root);

    QDirIterator dir_it(_root, QDir::Dirs, QDirIterator::Subdirectories);
    while (dir_it.hasNext())
    {
        QString p = dir_it.next();
        if (!p.endsWith("..") && !p.endsWith("."))
            _watcher->addPath(p);
    }

    QDirIterator file_it(_root, QStringList() << "*.flow", QDir::Files, QDirIterator::Subdirectories);
    while (file_it.hasNext())
    {
        QString p = file_it.next();
        _watcher->addPath(p);
        _files.push_back(p);

        load(p);
    }
}
GraphFileLoader::~GraphFileLoader()
{
    delete _watcher;
}
const char* GraphFileLoader::path() const
{
    return _root.toUtf8().constData();
}
void GraphFileLoader::dir_changed(const QString& path)
{
    QDirIterator dir_it(path, QDir::Dirs);
    while (dir_it.hasNext())
    {
        QString p = dir_it.next();
        if (!p.endsWith("..") && !p.endsWith("."))
            _watcher->addPath(p);
    }

    QDirIterator file_it(path, QStringList() << "*.flow", QDir::Files);
    while (file_it.hasNext())
    {
        QString p = file_it.next();
        if (!_files.contains(p))
        {
            _watcher->addPath(p);
            _files.push_back(p);

            load(p);
        }
    }
}
void GraphFileLoader::file_changed(const QString& path)
{
    load(path);
}
void GraphFileLoader::load(const QString& path)
{
    QFileInfo file_info(path);
    if (file_info.suffix() == "flow" && file_info.exists())
    {
        QString rel_path = QDir(_root).relativeFilePath(file_info.absoluteFilePath());
        QStringList split_path = rel_path.split('/');

        std::string class_name = "graph:";
        for (int i = 0; i < split_path.size() - 1; ++i)
        {
            class_name += split_path[i].toStdString();
            class_name += '.';
        }
        class_name += file_info.baseName().toStdString();

        // I can't be bothered with the warnings caused by Qts toupper/tolower
        std::transform(class_name.begin(), class_name.end(), class_name.begin(), ::tolower);

        QString title = FlowUIStyle::stylize_text(file_info.baseName().toUtf8().constData());
        QString category = "User/Graphs";

        for (int i = 0; i < split_path.size() - 1; ++i)
        {
            category.append('/' + FlowUIStyle::stylize_text(split_path[i].toUtf8().constData()));
        }

        RunGraphNode* node = python::make_object<RunGraphNode>();
        if (!node->load_graph(class_name.c_str(), path.toUtf8().constData()))
        {
            node->release();
            return;
        }
        node->set_attribute("title", title.toUtf8().constData());
        node->set_attribute("category", category.toUtf8().constData());

        FlowModule::instance().install_node_template(node);
        node->release();
    }
}

