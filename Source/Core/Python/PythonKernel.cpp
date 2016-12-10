#include "Common.h"
#include "Pluto/PlutoModule.h"
#include "PythonKernel.h"
#include "PythonModule.h"
#include "PythonCommon.h"
#include "StdStream.h"

#include "Object/PythonObject.h"

PythonKernel::PythonKernel() 
    : _main(nullptr),
    _stdout(nullptr),
    _stderr(nullptr)
{
}
PythonKernel::~PythonKernel()
{
}

void PythonKernel::start()
{
    Py_Initialize();
    PyEval_InitThreads();

    PythonType::ready_all();

    _main = new PythonModule(PyDict_GetItemString(PyImport_GetModuleDict(), "__main__"));

    PythonModule* pluto_module = new PlutoModule();
    pluto_module->create();
    install_module(pluto_module);
    _modules.push_back(pluto_module);

    PyObject* sys = PyImport_ImportModule("sys");

    _stdout = new PyStdStream();
    _stdout->addref();
    if (PyModule_AddObject(sys, "stdout", _stdout->python_object()) < 0)
        PyErr_Print();
    _stderr = new PyStdStream();
    _stderr->addref();
    if (PyModule_AddObject(sys, "stderr", _stderr->python_object()) < 0)
        PyErr_Print();
}
void PythonKernel::stop()
{
    if (_stdout)
    {
        _stdout->release();
        _stdout = nullptr;
    }
    if (_stderr)
    {
        _stderr->release();
        _stderr = nullptr;
    }

    Py_Finalize();

    for (PythonModule* m : _modules)
    {
        delete m;
    }
    _modules.clear();
    delete _main;
    _main = nullptr;

}

void PythonKernel::run_code(const std::string& source)
{
    assert(_main);
    if (!_main)
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
    
    PyObject* ret = PyRun_String(source.c_str(), inp, _main->dict(), _main->dict());
    if (!ret)
    {
        PyErr_Print();
    }
}
void PythonKernel::interrupt()
{
    PyErr_SetInterrupt();
}
void PythonKernel::install_module(PythonModule* module)
{
    PyDict_SetItemString(PyImport_GetModuleDict(), module->name(), module->module());
}

void PythonKernel::set_stdout_callback(OutputCallback* fn, void* data)
{
    if (_stdout)
        _stdout->set_callback(fn, data);
}
void PythonKernel::set_stderr_callback(OutputCallback* fn, void* data)
{
    if (_stderr)
        _stderr->set_callback(fn, data);
}

