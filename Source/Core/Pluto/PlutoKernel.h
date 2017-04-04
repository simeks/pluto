#ifndef __CORE_PLUTO_KERNEL_H__
#define __CORE_PLUTO_KERNEL_H__

#include <Core/API.h>
#include <Core/Python/Object.h>
#include <Core/Python/StdStream.h>

namespace python
{
    class Object;
}

class PlutoModuleCallback
{
public:
    virtual ~PlutoModuleCallback() {}
    virtual void print_html(const char* text) = 0;
};


class AutoReloader;
class FlowModule;
class ImageModule;
class PlutoModule;
class PythonModule;
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

    void add_auto_reload(const python::Object& module);

    void set_stdout_callback(OutputCallback* fn, void* data);
    void set_stderr_callback(OutputCallback* fn, void* data);
    void set_htmlout_callback(OutputCallback* fn, void* data);

    python::Object main_module();

private:
    void perform_startup();

    python::Object _main_module;
    
    python::Stream _stdout;
    python::Stream _stderr;
    python::Stream _htmlout;

    AutoReloader* _reloader;
};


#endif // __CORE_PLUTO_KERNEL_H__
