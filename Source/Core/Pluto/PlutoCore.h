#ifndef __CORE_PLUTO_CORE_H__
#define __CORE_PLUTO_CORE_H__

class PlutoKernel;
class PlutoKernelProxy;
class PythonModule;
class QThread;
class WindowManager;
class CORE_API PlutoCore
{
public:
    PlutoCore(int argc, char** argv);
    ~PlutoCore();

    void init();
    void shutdown();

    const char* user_dir() const;

    PlutoKernelProxy* kernel_proxy() const;
    QThread* kernel_thread() const;

    WindowManager* window_manager() const;

    static PlutoCore& instance();
    static void create(int argc, char** argv);
    static void destroy();
private:
    static PlutoCore* s_instance;

    PlutoKernel* _kernel;
    PlutoKernelProxy* _kernel_proxy;
    WindowManager* _window_manager;
};

#endif // __CORE_PLUTO_CORE_H__
