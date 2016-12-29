#ifndef __CORE_PLUTO_KERNEL_H__
#define __CORE_PLUTO_KERNEL_H__

#include <Core/API.h>

class PlutoModuleCallback
{
public:
    virtual void print_html(const char* text) = 0;
};

class FlowModule;
class ImageModule;
class PlutoModule;
class PythonModule;
class PyStdStream;
class CORE_API PlutoKernel
{
public:
    typedef void (OutputCallback)(void*, const char* msg);

    PlutoKernel();
    ~PlutoKernel();

    void prepare();
    void start();
    void stop();

    /// Run the specified code within the __main__ module
    void run_code(const std::string& source);

    /// Run the specified file within the __main__ module
    void run_file(const std::string& filename);
    
    void print(const std::string& text);
    void print_html(const std::string& text);
    void error(const std::string& text);

    void print_banner();

    /// Interrupts the kernel, aborting any ongoing execution. This may be called from any thread.
    void interrupt();

    void import_module(const std::string& module);

    void set_stdout_callback(OutputCallback* fn, void* data);
    void set_stderr_callback(OutputCallback* fn, void* data);
    void set_htmlout_callback(OutputCallback* fn, void* data);

private:
    void perform_startup();

    PythonModule* _main_module;
    
    PyStdStream* _stdout;
    PyStdStream* _stderr;
    PyStdStream* _htmlout;

};


#endif // __CORE_PLUTO_KERNEL_H__
