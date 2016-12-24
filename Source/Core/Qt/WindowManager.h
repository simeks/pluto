#ifndef __CORE_QT_WINDOW_MANAGER_H__
#define __CORE_QT_WINDOW_MANAGER_H__

#include <Core/Common.h>

#include <QObject.h>

struct WindowFactory
{
    virtual QObject* create() const = 0;
};

template<typename T>
struct TWindowFactory : public WindowFactory
{
    QObject* create() const { return new T(); }
};

class CORE_API WindowManager : public QObject
{
    Q_OBJECT

public:
    WindowManager();
    ~WindowManager();

    QObject* create_window(const WindowFactory& factory);

    template<typename T>
    T* create_window();

private slots:
    QObject* create_window_impl(const WindowFactory& factory);

};


template<typename T>
T* WindowManager::create_window()
{
    return (T*)create_window(TWindowFactory<T>());
}

#endif // __CORE_QT_WINDOW_MANAGER_H__
