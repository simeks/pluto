#include <Core/Common.h>

#include "PlutoKernelProxy.h"


namespace
{
    void kernel_stdout(void* data, const char* text)
    {
        PlutoKernelProxy* s = (PlutoKernelProxy*)data;
        s->on_stdout(text);

    }
    void kernel_stderr(void* data, const char* text)
    {
        PlutoKernelProxy* s = (PlutoKernelProxy*)data;
        s->on_stderr(text);
    }
    void kernel_htmlout(void* data, const char* text)
    {
        PlutoKernelProxy* s = (PlutoKernelProxy*)data;
        s->on_stdout(text, true);
    }
}

PlutoKernelProxy::PlutoKernelProxy(PlutoKernel* kernel) : _kernel(kernel)
{
    _kernel->set_stdout_callback(kernel_stdout, this);
    _kernel->set_stderr_callback(kernel_stderr, this);
    _kernel->set_htmlout_callback(kernel_htmlout, this);

    this->moveToThread(&_thread);
    _thread.start();
}
PlutoKernelProxy::~PlutoKernelProxy()
{
    _thread.quit();
    _thread.wait(1000);
    if (_thread.isRunning())
    {
        _thread.terminate();
        _thread.wait();
    }
}
void PlutoKernelProxy::prepare()
{
    _kernel->prepare();
}
void PlutoKernelProxy::start()
{
    _kernel->start();

    emit ready();
}
void PlutoKernelProxy::stop()
{
    _kernel->stop();
    delete _kernel;
}
void PlutoKernelProxy::on_stdout(const char* text, bool html)
{
    emit output(text, html);
}
void PlutoKernelProxy::on_stderr(const char* text)
{
    emit error_output(text);
}
void PlutoKernelProxy::invoke(const QString& command)
{
    _kernel->run_code(command.toStdString());
    emit ready();
}
void PlutoKernelProxy::interrupt()
{
    _kernel->interrupt();
}

