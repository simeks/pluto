#include <Core/Common.h>

#include "CommandInvoker.h"

#include <Core/Python/PythonKernel.h>

#include <Core/Platform/WindowsWrapper.h>


namespace
{
    void kernel_stdout(void* data, const char* text)
    {
        CommandInvoker* s = (CommandInvoker*)data;
        s->on_stdout(text);

    }
    void kernel_stderr(void* data, const char* text)
    {
        CommandInvoker* s = (CommandInvoker*)data;
        s->on_stderr(text);
    }
}

CommandInvoker::CommandInvoker()
{
    this->moveToThread(&_thread);
    _thread.start();

}
CommandInvoker::~CommandInvoker()
{
    _thread.quit();
    _thread.wait(1000);
    if (_thread.isRunning())
    {
        _thread.terminate();
        _thread.wait();
    }
}
void CommandInvoker::start()
{
    _kernel = new PythonKernel();
    _kernel->start();

    _kernel->set_stdout_callback(kernel_stdout, this);
    _kernel->set_stderr_callback(kernel_stderr, this);

    emit output("Kernel");
    emit ready();
}
void CommandInvoker::stop()
{
    _kernel->set_stdout_callback(nullptr, nullptr);
    _kernel->set_stderr_callback(nullptr, nullptr);

    _kernel->stop();
    delete _kernel;
}
void CommandInvoker::on_stdout(const char* text)
{
    emit output(text);
}
void CommandInvoker::on_stderr(const char* text)
{
    emit output(text);
}
void CommandInvoker::invoke(const QString& command)
{
    _kernel->run_code(command.toStdString());
    emit ready();
}
void CommandInvoker::interrupt()
{
    _kernel->interrupt();
}

