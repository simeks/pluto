#include <Testing/Framework.h>

#include <Core/Python/Class.h>
#include <Core/Python/PythonCommon.h>
#include <Core/Python/Function.h>
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
        ASSERT_EQUAL(python::from_python<int>(python::getattr(m, "constant_int")), 5);
        ASSERT_EQUAL_STR(python::from_python<const char*>(python::getattr(m, "constant_str")), "testtest");
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

//#define PYTHON_CLASS()
//#define PYTHON_CLASS_IMPLEMENT(cls, name) void python_init_class_##name(const python::Class& cls)
//
//namespace
//{
//    class TestClass
//    {
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
//PYTHON_CLASS_IMPLEMENT(TestClass, "TestClass")
//{
//    def(cls, )
//}

namespace
{
    int _class_method_flag_a = 0;
    int _class_method_flag_b = 0;
}

void testfn(const python::Object& obj)
{
    _class_method_flag_a = python::from_python<int>(python::getattr(obj, "a"));
    _class_method_flag_b = python::from_python<int>(python::getattr(obj, "b"));
}

PYTHON_MODULE(py_class_test)
{
    python::Object cls = python::Class("test", nullptr);
    python::def(cls, "testfn", python::make_function(testfn, "testfn"));

    python::setattr(module, "Cls", cls);
    python::setattr(cls, "a", python::to_python(143));
    def(module, "testfn", &testfn);
}

TEST_CASE(python_class)
{
    PYTHON_MODULE_INSTALL(py_class_test);
    Py_Initialize();
    {
        PyRun_SimpleString(
            "import py_class_test as p\n"
            "A = p.Cls\n"
            "a = A()\n"
            "a.b=1001\n"
            "a.testfn()\n"
        );
        ASSERT_EXPR(!PyErr_Occurred());
        ASSERT_EQUAL(_class_method_flag_a, 143);
        ASSERT_EQUAL(_class_method_flag_b, 1001);
        if (PyErr_Occurred())
            PyErr_Print();

    }
    Py_Finalize();
}

TEST_CASE(python_tuple)
{
    Py_Initialize();
    {
        Tuple t(4);
        ASSERT_EQUAL(t.size(), 4);
        t.set(0, 777);
        t.set(1, 77.7f);
        t.set(2, "asd");
        t.set(3, true);

        ASSERT_EXPR(t.get(0).is_instance(&PyLong_Type));
        ASSERT_EQUAL(t.get<int>(0), 777);

        ASSERT_EXPR(t.get(1).is_instance(&PyFloat_Type));
        ASSERT_EQUAL_F(t.get<float>(1), 77.7f, 0.0001f);

        ASSERT_EXPR(t.get(2).is_instance(&PyUnicode_Type));
        ASSERT_EQUAL_STR(t.get<const char*>(2), "asd");

        ASSERT_EXPR(t.get(3).is_instance(&PyBool_Type));
        ASSERT_EQUAL(t.get<bool>(3), true);
    }
    Py_Finalize();
}

TEST_CASE(python_dict)
{
    Py_Initialize();
    {
        Dict d;

        d.set("a", 777);
        d.set("b", 77.7f);
        d.set("c", "asd");
        d.set("d", true);

        ASSERT_EXPR(d.get("a").is_instance(&PyLong_Type));
        ASSERT_EQUAL(d.get<int>("a"), 777);

        ASSERT_EXPR(d.get("b").is_instance(&PyFloat_Type));
        ASSERT_EQUAL_F(d.get<float>("b"), 77.7f, 0.0001f);

        ASSERT_EXPR(d.get("c").is_instance(&PyUnicode_Type));
        ASSERT_EQUAL_STR(d.get<const char*>("c"), "asd");

        ASSERT_EXPR(d.get("d").is_instance(&PyBool_Type));
        ASSERT_EQUAL(d.get<bool>("d"), true);
    }
    Py_Finalize();
}


