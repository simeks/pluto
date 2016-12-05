#ifndef __CORE_PYTHON_KERNEL_H__
#define __CORE_PYTHON_KERNEL_H__

#include <Core/API.h>


class CORE_API PythonKernel
{
public:
    typedef void (OutputCallback)(void*, const char* msg);

    PythonKernel();
    ~PythonKernel();

    void start();
    void stop();

    void run_code(const std::string& source);
    
    /// Interrupts the kernel, aborting any ongoing execution. This may be called from any thread.
    void interrupt();

    void set_stdout_callback(OutputCallback* fn, void* data);
    void set_stderr_callback(OutputCallback* fn, void* data);

private:
    PyObject* _module;
    PyObject* _stdout;
    PyObject* _stderr;

};


#endif // __CORE_PYTHON_KERNEL_H__
