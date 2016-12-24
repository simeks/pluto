#include "Common.h"

#include "WindowManager.h"

WindowManager::WindowManager()
{
}
WindowManager::~WindowManager()
{
}
QObject* WindowManager::create_window(const WindowFactory& factory)
{
    QObject* obj;
    QMetaObject::invokeMethod(this, "create_window_impl", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QObject*, obj), Q_ARG(const WindowFactory&, factory));
    return obj;
}
QObject* WindowManager::create_window_impl(const WindowFactory& factory)
{
    return factory.create();
}
