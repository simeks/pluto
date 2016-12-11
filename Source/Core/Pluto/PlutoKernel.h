#ifndef __CORE_PLUTO_KERNEL_H__
#define __CORE_PLUTO_KERNEL_H__

#include <Core/API.h>

class PlutoModuleCallback
{
public:
    virtual void print_html(const char* text) = 0;
};

class PythonModule;
class PyStdStream;
class CORE_API PlutoKernel
{
public:
    typedef void (OutputCallback)(void*, const char* msg);

    PlutoKernel(PlutoModuleCallback* callback);
    ~PlutoKernel();

    void start();
    void stop();

    void run_code(const std::string& source);
    
    void print(const std::string& text);
    void print_html(const std::string& text);
    void error(const std::string& text);

    void print_banner();

    /// Interrupts the kernel, aborting any ongoing execution. This may be called from any thread.
    void interrupt();

    void install_module(PythonModule* module);
    void import_module(const std::string& module);

    void set_stdout_callback(OutputCallback* fn, void* data);
    void set_stderr_callback(OutputCallback* fn, void* data);
    void set_htmlout_callback(OutputCallback* fn, void* data);

private:
    PythonModule* _main_module;
    PythonModule* _pluto_module;

    PyStdStream* _stdout;
    PyStdStream* _stderr;
    PyStdStream* _htmlout;

};


#endif // __CORE_PLUTO_KERNEL_H__
