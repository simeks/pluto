#include <Core/Common.h>

#include "PlutoKernelRunner.h"


namespace
{
    void kernel_stdout(void* data, const char* text)
    {
        PlutoKernelRunner* s = (PlutoKernelRunner*)data;
        s->on_stdout(text);

    }
    void kernel_stderr(void* data, const char* text)
    {
        PlutoKernelRunner* s = (PlutoKernelRunner*)data;
        s->on_stderr(text);
    }
    void kernel_htmlout(void* data, const char* text)
    {
        PlutoKernelRunner* s = (PlutoKernelRunner*)data;
        s->on_stdout(text, true);
    }
}


PlutoKernelRunner::CallbackProxy::CallbackProxy(PlutoKernelRunner* owner) 
    : _owner(owner)
{
}

void PlutoKernelRunner::CallbackProxy::print_html(const char* text)
{
    emit _owner->output(text, true);
}

PlutoKernelRunner::PlutoKernelRunner() : _callback_proxy(new CallbackProxy(this))
{
    this->moveToThread(&_thread);
    _thread.start();

}
PlutoKernelRunner::~PlutoKernelRunner()
{
    _thread.quit();
    _thread.wait(1000);
    if (_thread.isRunning())
    {
        _thread.terminate();
        _thread.wait();
    }
}
void PlutoKernelRunner::start()
{
    _kernel = new PlutoKernel(_callback_proxy);
    _kernel->set_stdout_callback(kernel_stdout, this);
    _kernel->set_stderr_callback(kernel_stderr, this);
    _kernel->set_htmlout_callback(kernel_htmlout, this);

    _kernel->start();

    emit ready();
}
void PlutoKernelRunner::stop()
{
    _kernel->stop();
    delete _kernel;
}
void PlutoKernelRunner::on_stdout(const char* text, bool html)
{
    emit output(text, html);
}
void PlutoKernelRunner::on_stderr(const char* text)
{
    emit error_output(text);
}
void PlutoKernelRunner::invoke(const QString& command)
{
    _kernel->run_code(command.toStdString());
    emit ready();
}
void PlutoKernelRunner::interrupt()
{
    _kernel->interrupt();
}

