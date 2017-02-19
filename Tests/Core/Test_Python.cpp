#include <Testing/Framework.h>

#include <Core/Python/Module.h>
#include <Core/Python/Object.h>
#include <Core/Python/NumPy.h>

using namespace testing;

namespace
{
    bool _function_void_flag = false;
    int _function_1arg_flag = 0;
    int _function_2arg_flag_a = 0;
    int _function_2arg_flag_b = 0;
    bool _function_2arg_return_flag = false;
}

void function_void()
{
    _function_void_flag = true;
}
void function_1arg(int a)
{
    _function_1arg_flag = a;
}
void function_2arg(int a, int b)
{
    _function_2arg_flag_a = a;
    _function_2arg_flag_b = b;
}
int function_2arg_return(int a, int b)
{
    _function_2arg_return_flag = true;
    return a + b;
}

PYTHON_MODULE(py_test_module)
{
    def(module, "constant_int", 5);
    def(module, "constant_str", "testtest");

    def(module, "function_void", &function_void);
    def(module, "function_1arg", &function_1arg);
    def(module, "function_2arg", &function_2arg);
    def(module, "function_2arg_return", &function_2arg_return);
}


TEST_CASE(python_module)
{
    PYTHON_MODULE_INSTALL(py_test_module);

    Py_Initialize();

    python::Object m = python::import("py_test_module");
    ASSERT_EQUAL(python::getattr(m, "constant_int", 0), 5);
    ASSERT_EQUAL_STR(python::getattr(m, "constant_str", ""), "testtest");
}

TEST_CASE(python_function)
{
    PYTHON_MODULE_INSTALL(py_test_module);

    Py_Initialize();

    const char* script =
        "import py_test_module as m\n"
        "m.function_void()\n"
        "m.function_1arg(5)\n"
        "m.function_2arg(7, 9)\n"
        "if m.function_2arg_return(15, 30) != 45:\n"
        "    raise ValueError('function_2arg_return(15, 30) != 45')\n"
        "\n"
        ;
    PyRun_SimpleString(script);
    ASSERT_EQUAL(_function_void_flag, true);
    ASSERT_EQUAL(_function_1arg_flag, 5);
    ASSERT_EQUAL(_function_2arg_flag_a, 7);
    ASSERT_EQUAL(_function_2arg_flag_b, 9);
    ASSERT_EQUAL(_function_2arg_return_flag, true);
    ASSERT_EXPR(!PyErr_Occurred());
    if (PyErr_Occurred())
        PyErr_Print();
}
