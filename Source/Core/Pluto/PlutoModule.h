#ifndef __CORE_PLUTO_MODULE_H__
#define __CORE_PLUTO_MODULE_H__

class Object;
class PyStdStream;

namespace python
{
    class Tuple;
}

namespace pluto
{
    CORE_API const char* user_dir();
    CORE_API const char* python_dir();
    CORE_API const char* module_dir();

    CORE_API void run_file(const std::string& file);

    /// Enables auto reloading of the given module. Whenever the python file containing
    ///     the given module is changed the module is reimported.
    CORE_API void auto_reload(const python::Object& module);

    /// Enables or disables the auto reloading, this is to avoid problems with python 
    ///     importing modules during finalization
    /// Disabled by default
    CORE_API void enable_auto_reload(bool enable);

    CORE_API void install_python_module();
}

#endif // __CORE_PLUTO_MODULE_H__
