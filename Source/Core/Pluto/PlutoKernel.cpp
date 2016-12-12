#include "Common.h"
#include "PlutoKernel.h"
#include "PlutoModule.h"
#include "Python/NumPy.h"
#include "Python/PythonModule.h"
#include "Python/PythonCommon.h"
#include "Python/StdStream.h"

PlutoKernel::PlutoKernel(PlutoModuleCallback* callback)
    : _main_module(nullptr), _pluto_module(nullptr)
{
    _pluto_module = new PlutoModule(callback);

    _stdout = new PyStdStream();
    _htmlout = new PyStdStream();
    _stderr = new PyStdStream();
}
PlutoKernel::~PlutoKernel()
{
}

void PlutoKernel::start()
{
    Py_Initialize();
    PyEval_InitThreads();

    numpy::initialize();

    PythonType::ready_all();

    _main_module = new PythonModule(PyDict_GetItemString(PyImport_GetModuleDict(), "__main__"));

    _pluto_module->create();
    install_module(_pluto_module);

    PyObject* sys = PyImport_ImportModule("sys");

    _stdout->addref();
    if (PyModule_AddObject(sys, "stdout", _stdout->python_object()) < 0)
        PyErr_Print();
    _stderr->addref();
    if (PyModule_AddObject(sys, "stderr", _stderr->python_object()) < 0)
        PyErr_Print();

    print_banner();

    import_module("pluto");
}
void PlutoKernel::stop()
{
    if (_stdout)
        _stdout->release();
    if (_htmlout)
        _htmlout->release();
    if (_stderr)
        _stderr->release();
    
    Py_Finalize();

    delete _pluto_module;
    _pluto_module = nullptr;
    delete _main_module;
    _main_module = nullptr;
}

void PlutoKernel::run_code(const std::string& source)
{
    assert(_main_module);
    if (!_main_module)
        return;
    
    int inp = Py_single_input;
    for (char c : source)
    {
        if (c == '\n')
        {
            inp = Py_file_input;
            break;
        }
    }
    
    PyObject* ret = PyRun_String(source.c_str(), inp, _main_module->dict(), _main_module->dict());
    if (!ret)
    {
        PyErr_Print();
    }
}
void PlutoKernel::print(const std::string& text)
{
    PySys_WriteStdout(text.c_str());
}
void PlutoKernel::print_html(const std::string& text)
{
    _htmlout->write(text.c_str());
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
void PlutoKernel::install_module(PythonModule* module)
{
    PyDict_SetItemString(PyImport_GetModuleDict(), module->name(), module->module());
}
void PlutoKernel::import_module(const std::string& module)
{
    print("Importing module '" + module + "'.");
    PyObject* m = PyImport_ImportModule(module.c_str());
    if (!m)
    {
        PyErr_Print();
        return;
    }
    _main_module->add_object(module.c_str(), m);
}

void PlutoKernel::set_stdout_callback(OutputCallback* fn, void* data)
{
    if (_stdout)
        _stdout->set_callback(fn, data);
}
void PlutoKernel::set_htmlout_callback(OutputCallback* fn, void* data)
{
    if (_htmlout)
        _htmlout->set_callback(fn, data);
}
void PlutoKernel::set_stderr_callback(OutputCallback* fn, void* data)
{
    if (_stderr)
        _stderr->set_callback(fn, data);
}

