#ifndef __CORE_PLUTO_KERNEL_RUNNER_H__
#define __CORE_PLUTO_KERNEL_RUNNER_H__

#include <QObject>
#include <QThread>
#include <Core/Common.h>

#include <Core/Pluto/PlutoKernel.h>

class CORE_API PlutoKernelProxy : public QObject
{
    Q_OBJECT;
public:
    PlutoKernelProxy(PlutoKernel* kernel);
    ~PlutoKernelProxy();

    void on_stdout(const char* text, bool html = false);
    void on_stderr(const char* text);

    public slots:
    void start();
    void stop();

    void invoke(const QString& command);
    /// Interrupts the kernel, aborting any ongoing execution. This may be called from any thread.
    void interrupt();

signals:
    void ready();
    void busy();

    void output(const QString& text, bool html);
    void error_output(const QString& text);

private:
    class CallbackProxy : public PlutoModuleCallback
    {
    public:
        CallbackProxy(PlutoKernelProxy* owner);

        void print_html(const char* text) OVERRIDE;

    private:
        PlutoKernelProxy* _owner;
    };

    PlutoKernel* _kernel;
    CallbackProxy* _callback_proxy;
    QThread _thread;
};


#endif // __CORE_PLUTO_KERNEL_RUNNER_H__
