#ifndef __CORE_PLUTO_MODULE_H__
#define __CORE_PLUTO_MODULE_H__

#include <Core/Python/PythonModule.h>

class PyStdStream;
class PlutoModule : public PythonModule
{
public:
    PlutoModule();
    virtual ~PlutoModule();

    virtual void init_module() OVERRIDE;

    const char* get_user_dir();

    void print_html(const std::string& txt);

    PyObject* register_class(PyObject* cls);
    PyObject* classes() const;
    
    Object* create_object(const Tuple& args);

private:
    static std::string s_version;
};

#endif // __CORE_PLUTO_MODULE_H__
