#ifndef __PLUTO_COMMAND_INVOKER_H__
#define __PLUTO_COMMAND_INVOKER_H__

#include <QObject>
#include <QThread>

class PythonKernel;
class CommandInvoker : public QObject
{
    Q_OBJECT;
public:
    CommandInvoker();
    ~CommandInvoker();

    void on_stdout(const char* text);
    void on_stderr(const char* text);

public slots:
    void start();
    void stop();

    void invoke(const QString& command);
    /// Interrupts the kernel, aborting any ongoing execution. This may be called from any thread.
    void interrupt();

signals:
    void ready();
    void output(const QString& text);
    void error_output(const QString& text);

private:
    PythonKernel* _kernel;
    QThread _thread;
};


#endif // __PLUTO_COMMAND_INVOKER_H__
