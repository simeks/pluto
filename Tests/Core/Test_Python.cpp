#include <Testing/Framework.h>

#include <Core/Python/Class.h>
#include <Core/Python/PythonCommon.h>
#include <Core/Python/Function.h>
#include <Core/Python/Module.h>
#include <Core/Python/NumPy.h>
#include <Core/Python/Object.h>

#include "Test_Python.h"

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
const char* function_return_str()
{
    return "teststring";
}
std::string function_return_stdstr()
{
    return "stdteststring";
}

PYTHON_MODULE(py_test_module)
{
    module.def("constant_int", 5);
    module.def("constant_str", "testtest");

    module.def("function_void", &function_void);
    module.def("function_1arg", &function_1arg);
    module.def("function_2arg", &function_2arg);
    module.def("function_2arg_return", &function_2arg_return);
    module.def("function_return_str", &function_return_str);
    module.def("function_return_stdstr", &function_return_stdstr);
}


TEST_CASE(python_module)
{
    PYTHON_MODULE_INSTALL(py_test_module);
    PYTHON_TEST_PREPARE();
    {
        python::Object m = python::import("py_test_module");
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL(python::from_python<int>(python::getattr(m, "constant_int").ptr()), 5);
        ASSERT_EQUAL_STR(python::from_python<const char*>(python::getattr(m, "constant_str").ptr()), "testtest");
    }
    PYTHON_TEST_CLEANUP();
}

TEST_CASE(python_function)
{
    PYTHON_MODULE_INSTALL(py_test_module);
    PYTHON_TEST_PREPARE();
    {
        const char* script =
            "import py_test_module as m\n"
            "m.function_void()\n"
            "m.function_1arg(5)\n"
            "m.function_2arg(7, 9)\n"
            "if m.function_2arg_return(15, 30) != 45:\n"
            "    raise ValueError('function_2arg_return(15, 30) != 45')\n"
            "if m.function_return_str() != 'teststring':\n"
            "    raise ValueError('function_return_str() != \\'teststring\\'')\n"
            "if m.function_return_stdstr() != 'stdteststring':\n"
            "    raise ValueError('function_return_stdstr() != \\'stdteststring\\'')\n"
            "\n"
            ;
        PyRun_SimpleString(script);
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL(_function_void_flag, true);
        ASSERT_EQUAL(_function_1arg_flag, 5);
        ASSERT_EQUAL(_function_2arg_flag_a, 7);
        ASSERT_EQUAL(_function_2arg_flag_b, 9);
        ASSERT_EQUAL(_function_2arg_return_flag, true);
    }

    PYTHON_TEST_CLEANUP();
}

namespace
{
    int _class_some_value_1 = 0;
    int _class_some_value_2 = 0;
}

struct TestClass
{
    int _a;

    TestClass(int a) : _a(a) { _class_some_value_1 = a;  }

    int some_method(int a)
    {
        _a = a;
        _class_some_value_2 = a;
        return _a + 10;
    }
};

struct BaseClass
{
    int some_method()
    {
        return 1;
    }
};

struct TestClassInit
{
    TestClassInit(int a) 
    { 
        _class_some_value_1 = a; 
    }
};
struct TestClassInitVarargs
{
    TestClassInitVarargs(const Tuple& args)
    {
        _class_some_value_1 = args.get<int>(0);
        _class_some_value_2 = args.get<int>(1);
    }
};
struct TestClassInitVarargsKeywords
{
    TestClassInitVarargsKeywords(const Tuple& args, const Dict& kw)
    {
        _class_some_value_1 = args.get<int>(0);
        _class_some_value_2 = kw.get<int>("b");
    }
};


PYTHON_MODULE(py_class_test)
{
    python::Class cls = python::make_class<TestClass>("TestClass");
    cls.def_init<TestClass, int>();
    cls.def("some_method", &TestClass::some_method);

    python::Class cls_init = python::make_class<TestClassInit>("TestClassInit");
    cls_init.def_init<TestClassInit, int>();

    python::Class cls_init_args = python::make_class<TestClassInitVarargs>("TestClassInitVarargs");
    cls_init_args.def_init_varargs<TestClassInitVarargs>();

    python::Class cls_init_args_kw = python::make_class<TestClassInitVarargsKeywords>("TestClassInitVarargsKeywords");
    cls_init_args_kw.def_init_varargs_keywords<TestClassInitVarargsKeywords>();

    module.def("TestClass", cls);
    module.def("TestClassInit", cls_init);
    module.def("TestClassInitVarargs", cls_init_args);
    module.def("TestClassInitVarargsKeywords", cls_init_args_kw);

}

TEST_CASE(python_class_init)
{
    PYTHON_MODULE_INSTALL(py_class_test);
    PYTHON_TEST_PREPARE();
    {
        PyRun_SimpleString(
            "import py_class_test as p\n"
            "a = p.TestClassInit(321)\n"
        );
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL(_class_some_value_1, 321);

        PyRun_SimpleString(
            "import py_class_test as p\n"
            "a = p.TestClassInitVarargs(222, 333)\n"
        );
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL(_class_some_value_1, 222);
        ASSERT_EQUAL(_class_some_value_2, 333);


        PyRun_SimpleString(
            "import py_class_test as p\n"
            "a = p.TestClassInitVarargsKeywords(444, b=555)\n"
        );
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL(_class_some_value_1, 444);
        ASSERT_EQUAL(_class_some_value_2, 555);
    }
    PYTHON_TEST_CLEANUP();
}

TEST_CASE(python_class_inheritance)
{
    PYTHON_MODULE_INSTALL(py_class_test);
    PYTHON_TEST_PREPARE();
    {
        PyRun_SimpleString(
            "import py_class_test as p\n"
            "a = p.TestClassInit(321)\n"
        );

        ASSERT_NO_PYTHON_ERROR();
    }
    PYTHON_TEST_CLEANUP();
}

TEST_CASE(python_class_method)
{
    PYTHON_MODULE_INSTALL(py_class_test);
    PYTHON_TEST_PREPARE();
    {
        PyRun_SimpleString(
            "import py_class_test as p\n"
            "A = p.TestClass\n"
            "a = A(1)\n"
            "ret = a.some_method(200)\n"
            "if ret != 210:\n"
            "    raise ValueError('Expected 210, got %d' % ret)\n"
        );
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL(_class_some_value_2, 200);
    }
    PYTHON_TEST_CLEANUP();
}

TEST_CASE(python_class_make_instance)
{
    PYTHON_MODULE_INSTALL(py_class_test);
    PYTHON_TEST_PREPARE();
    {
        // TestClass is defined in module
        python::import("py_class_test");

        TestClass data(567);
        python::Object instance = python::make_instance<TestClass>(&data);
        ASSERT_NO_PYTHON_ERROR();

        TestClass* converted = python::from_python<TestClass*>(instance);
        ASSERT_NO_PYTHON_ERROR();

        ASSERT_EQUAL(converted->_a, data._a);

    }
    PYTHON_TEST_CLEANUP();
}

TEST_CASE(python_class_cast)
{
    PYTHON_MODULE_INSTALL(py_class_test);
    PYTHON_TEST_PREPARE();
    {
        // TestClass is defined in module
        python::import("py_class_test");

        TestClass data(567);
        python::Object instance = python::make_instance<TestClass>(&data);
        ASSERT_NO_PYTHON_ERROR();

        TestClass converted = python::from_python<TestClass>(instance);
        ASSERT_NO_PYTHON_ERROR();

        ASSERT_EQUAL(converted._a, data._a);

    }
    PYTHON_TEST_CLEANUP();
}


TEST_CASE(python_tuple)
{
    PYTHON_TEST_PREPARE();
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

        ASSERT_NO_PYTHON_ERROR();
    }
    PYTHON_TEST_CLEANUP();
}

TEST_CASE(python_dict)
{
    PYTHON_TEST_PREPARE();
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

        ASSERT_NO_PYTHON_ERROR();
    }
    PYTHON_TEST_CLEANUP();
}

void raise_error()
{
    PYTHON_ERROR(PyExc_ValueError, "Error");
}

PYTHON_MODULE(py_error_test)
{
    module.def("raise_error", &raise_error);
}

TEST_CASE(python_error)
{
    PYTHON_MODULE_INSTALL(py_error_test);
    PYTHON_TEST_PREPARE();
    {
        PyRun_SimpleString(
            "import py_error_test as p\n"
            "p.raise_error()\n"
        );
        ASSERT_NO_PYTHON_ERROR();
    }
    PYTHON_TEST_CLEANUP();
}
