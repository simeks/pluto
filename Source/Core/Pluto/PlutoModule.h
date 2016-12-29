#ifndef __CORE_PLUTO_MODULE_H__
#define __CORE_PLUTO_MODULE_H__

#include <Core/Python/PythonModule.h>

class PyStdStream;
class PlutoModule : public PythonModuleHelper<PlutoModule>
{
public:
    PlutoModule();
    virtual ~PlutoModule();

    virtual void post_init() OVERRIDE;

    const char* get_user_dir();

    void print_html(const std::string& txt);

    PyObject* register_class(PyObject* cls);
    PyObject* classes() const;
    
    Object* create_object(const Tuple& args);

    static const char* name();
private:
    static std::string s_version;
};

#endif // __CORE_PLUTO_MODULE_H__
