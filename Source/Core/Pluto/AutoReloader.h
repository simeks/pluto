#ifndef __CORE_PLUTO_AUTO_RELOADER_H__
#define __CORE_PLUTO_AUTO_RELOADER_H__

#include <Core/Common.h>
#include <QObject>

namespace python
{
    class Object;
}

class QFileSystemWatcher;
class CORE_API AutoReloader : public QObject
{
    Q_OBJECT;
public:
    AutoReloader();
    ~AutoReloader();

    void add_module(const python::Object& module);

public slots:
    void file_changed(const QString& path);

private:
    QFileSystemWatcher* _watcher;
    std::map<QString, python::Object> _modules;
};


#endif // __CORE_PLUTO_AUTO_RELOADER_H__
