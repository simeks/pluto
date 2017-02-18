#ifndef __CORE_PLUTO_MODULE_H__
#define __CORE_PLUTO_MODULE_H__

class Object;
class PyStdStream;
class Tuple;

namespace pluto
{
    const char* user_dir();
    const char* python_dir();
    const char* module_dir();

    void run_file(const std::string& file);

    python::Object register_class(const python::Object& cls);
    python::Object classes();

    Object* create_object(const Tuple& args);

    void auto_reload(const python::Object& module);

    void install_python_module();
}

#endif // __CORE_PLUTO_MODULE_H__
