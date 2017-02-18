#ifndef __QT_GRAPH_FILE_LOADER_H__
#define __QT_GRAPH_FILE_LOADER_H__

#include "API.h"
#include <QObject>

class QFileSystemWatcher;
class FLOW_API GraphFileLoader : public QObject
{
    Q_OBJECT
public:
    GraphFileLoader(const char* path);
    ~GraphFileLoader();

    const char* path() const;

public slots:
    void dir_changed(const QString& path);
    void file_changed(const QString& path);


private:
    void load(const QString& path);

    QFileSystemWatcher* _watcher;
    
    QString _root;

    QStringList _files;
};

#endif // __QT_GRAPH_FILE_LOADER_H__
