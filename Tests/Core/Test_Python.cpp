#include <Testing/Framework.h>

#include <Core/Python/Class.h>
#include <Core/Python/Module.h>
#include <Core/Python/NumPy.h>
#include <Core/Python/Object.h>

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
    {
        python::Object m = python::import("py_test_module");
        ASSERT_EQUAL(python::getattr(m, "constant_int", 0), 5);
        ASSERT_EQUAL_STR(python::getattr(m, "constant_str", ""), "testtest");
    }
    Py_Finalize();
}

TEST_CASE(python_function)
{
    PYTHON_MODULE_INSTALL(py_test_module);
    Py_Initialize();
    {

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

    Py_Finalize();
}
//
//#define PYTHON_CLASS()
//#define PYTHON_CLASS_IMPLEMENT(name) void python_init_class_##name(const python::Class& cls)
//
//namespace
//{
//    class TestClass
//    {
//        PYTHON_CLASS();
//
//    public:
//        TestClass() {}
//        ~TestClass() {}
//
//        void hey() 
//        {
//            std::cout << "hey" << std::endl;
//        }
//    };
//}
//
//PYTHON_CLASS_IMPLEMENT(TestClass)
//{
//    def(cls, )
//}
//

void testfn()
{
}

PYTHON_MODULE(py_class_test)
{
    def(module, "testfn", &testfn);
}

TEST_CASE(python_class)
{
    PYTHON_MODULE_INSTALL(py_class_test);
    Py_Initialize();
    {
        PyRun_SimpleString(
            "import py_class_test as p\n"
            "class A:\n"
            "   pass\n"
            "A.fn = p.testfn\n"
            "a = A()\n"
            "print(a.fn, type(a.fn))\n"
            "a.fn()\n"
        );
        ASSERT_EXPR(!PyErr_Occurred());
        if (PyErr_Occurred())
            PyErr_Print();

        PyErr_Print();
    }

    Py_Finalize();
}

