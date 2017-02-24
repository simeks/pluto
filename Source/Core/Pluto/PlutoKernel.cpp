#include "Common.h"

#include "AutoReloader.h"
#include "Image/ImageModule.h"
#include "Object/PythonClass.h"
#include "Platform/FilePath.h"
#include "Platform/FileUtil.h"
#include "PlutoCore.h"
#include "PlutoKernel.h"
#include "PlutoModule.h"
#include "Python/Class.h"
#include "Python/NumPy.h"
#include "Python/Module.h"
#include "Python/PythonCommon.h"
#include "Python/StdStream.h"

#include <Flow/FlowModule.h>

#include <fstream>

PlutoKernel::PlutoKernel()
    : _reloader(nullptr)
{
    pluto::install_python_module();
    image::install_python_module();
}
PlutoKernel::~PlutoKernel()
{
    delete _reloader;

    python::Object sys = python::import("sys");
    python::setattr(sys, "stdout", Py_None);
    python::setattr(sys, "stderr", Py_None);

    //if (_stdout)
    //    _stdout->release();
    //if (_htmlout)
    //    _htmlout->release();
    //if (_stderr)
    //    _stderr->release();
    
    _main_module = Py_None;
}
void PlutoKernel::prepare()
{
    Py_Initialize();
    PythonClass::ready_all();

    //_stdout = object_new<PyStdStream>();
    //_htmlout = object_new<PyStdStream>();
    //_stderr = object_new<PyStdStream>();

    python::Object sys = python::import("sys");
    /*_stdout->addref();
    python::setattr(sys, "stdout", _stdout);
    _stderr->addref();
    python::setattr(sys, "stderr", _stderr);
*/
    python::Object path = python::getattr(sys, "path");
    PyList_Append(path.ptr(), PyUnicode_FromString(PlutoCore::instance().python_dir())); // TODO: List object
    PyList_Append(path.ptr(), PyUnicode_FromString(PlutoCore::instance().module_dir()));

    numpy::initialize();

    _main_module = PyDict_GetItemString(PyImport_GetModuleDict(), "__main__");
/*
    _htmlout->addref();*/

    //python::Object pluto_module = python::import("pluto_api");
    //python::setattr(pluto_module, "htmlout", _htmlout);

    _reloader = new AutoReloader();
}
void PlutoKernel::start()
{
    perform_startup();
}
void PlutoKernel::stop()
{
}

void PlutoKernel::run_code(const std::string& source)
{
    int inp = Py_single_input;
    for (char c : source)
    {
        if (c == '\n')
        {
            inp = Py_file_input;
            break;
        }
    }

    PyObject* ret = PyRun_String(source.c_str(), inp, python::get_dict(_main_module).ptr(), python::get_dict(_main_module).ptr());

    if (!ret)
    {
        PyErr_Print();
    }
}
void PlutoKernel::run_file(const std::string& filename)
{
    std::ifstream f(filename, std::ifstream::in);
    if (f.is_open())
    {
        std::stringstream buf;
        buf << f.rdbuf();

        PyRun_SimpleString(buf.str().c_str());
        f.close();
    }
    else
    {
        // TODO: Log that file was not run?
    }
}
void PlutoKernel::print(const std::string& text)
{
    PySys_WriteStdout(text.c_str());
}
void PlutoKernel::print_html(const std::string& text)
{
    text;//_htmlout->write(text.c_str());
}
void PlutoKernel::error(const std::string& text)
{
    PySys_WriteStderr(text.c_str());
}
void PlutoKernel::print_banner()
{
    std::stringstream ss;
    ss << "<span>Plumbing Toolbox - Running on Python " << PY_VERSION << "</span><br/><br/>";
    print_html(ss.str());
}
void PlutoKernel::interrupt()
{
    PyErr_SetInterrupt();
}
void PlutoKernel::add_auto_reload(const python::Object& module)
{
    _reloader->add_module(module);
}

void PlutoKernel::set_stdout_callback(OutputCallback* fn, void* data)
{
    fn; data;
    //if (_stdout)
    //    _stdout->set_callback(fn, data);
}
void PlutoKernel::set_htmlout_callback(OutputCallback* fn, void* data)
{
    fn; data;
    //if (_htmlout)
    //    _htmlout->set_callback(fn, data);
}
void PlutoKernel::set_stderr_callback(OutputCallback* fn, void* data)
{
    fn; data;
    //if (_stderr)
    //    _stderr->set_callback(fn, data);
}
python::Object PlutoKernel::main_module()
{
    return _main_module;
}
void PlutoKernel::perform_startup()
{
    print_banner();

    // Run startup script (if any)
    FilePath path(pluto::user_dir());
    path.join("startup.py");
    run_file(path.get());
}

