#include "Common.h"
#include "PythonWrapper.h"

#include "PythonKernel.h"
#include "PythonWrapper.h"
#include "StdStream.h"

PythonKernel::PythonKernel() 
    : _module(nullptr),
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

    _module = PyDict_GetItemString(PyImport_GetModuleDict(), "__main__");

    py_std_stream::init_type();

    PyObject* sys = PyImport_ImportModule("sys");

    _stdout = py_std_stream::create_stream();
    PyModule_AddObject(sys, "stdout", _stdout);
    _stderr = py_std_stream::create_stream();
    PyModule_AddObject(sys, "stderr", _stderr);
}
void PythonKernel::stop()
{
    _module = nullptr;
    Py_Finalize();
}

void PythonKernel::run_code(const std::string& source)
{
    assert(_module);
    if (!_module)
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
    
    PyObject* ret = PyRun_String(source.c_str(), inp, PyModule_GetDict(_module), PyModule_GetDict(_module));
    if (!ret)
    {
        PyErr_Print();
    }
    else
        std::cout << ((PyTypeObject*)PyObject_Type(ret))->tp_name << std::endl;
}
void PythonKernel::interrupt()
{
    PyErr_SetInterrupt();
}
void PythonKernel::set_stdout_callback(OutputCallback* fn, void* data)
{
    py_std_stream::set_callback(_stdout, fn, data);
}
void PythonKernel::set_stderr_callback(OutputCallback* fn, void* data)
{
    py_std_stream::set_callback(_stderr, fn, data);
}

