#ifndef __QT_GRAPH_FILE_RELOADER_H__
#define __QT_GRAPH_FILE_RELOADER_H__

#include "API.h"
#include <QObject>

class FlowPythonModule;
class QFileSystemWatcher;
class FLOW_API GraphFileReloader : public QObject
{
    Q_OBJECT
public:
    GraphFileReloader(FlowPythonModule* module);
    ~GraphFileReloader();

    void add_file(const char* class_name, const char* file);

    public slots:
    void file_changed(const QString& path);

private:
    FlowPythonModule* _module;
    QFileSystemWatcher* _watcher;

    std::map<QString, QString> _files; // filename => class name
};

#endif // __QT_GRAPH_FILE_RELOADER_H__
