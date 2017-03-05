#include <Testing/Framework.h>

#include <Core/Object/Object.h>
#include <Core/Python/Module.h>
#include <Core/Python/PythonFunction.h>

#include "Test_Python.h"

using namespace testing;

#define NO_API 

class TestClass : public Object
{
    DECLARE_OBJECT(TestClass, Object);

public:
    DECLARE_OBJECT_CONSTRUCTOR(TestClass);
    ~TestClass() {}

    void test() {}

};

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0(TestClass, test);

OBJECT_INIT_TYPE_FN(TestClass)
{
        OBJECT_PYTHON_ADD_METHOD(TestClass, test, "");
}

IMPLEMENT_OBJECT(TestClass, "TestClass", NO_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(TestClass, Object);

PYTHON_MODULE(object_test)
{
    python::def(module, "TestClass", TestClass::static_class());
}

TEST_CASE(python_class_old)
{
    PYTHON_MODULE_INSTALL(object_test);

    Py_Initialize();
    {
        TestClass::static_class()->ready();

        TestClass* obj = object_new<TestClass>();
        ASSERT_NO_PYTHON_ERROR();
        PyRun_SimpleString(
            "import object_test as t\n"
            "A = t.TestClass\n"
            "a = A()\n"
        );
        obj;

    }
    Py_Finalize();
}

#include <Core/Pluto/PlutoModule.h>
#include <Flow/FlowPythonModule.h>

void testfn(TestClass* cls)
{
    cls;
}

PYTHON_MODULE(pluto_module)
{
    python::def(module, "register_class", &pluto::register_class, "register_class(cls)");
    python::def(module, "install", &testfn, "testfn(cls)");

}

// Objects are about to be removed so we don't care
//TEST_CASE(python_class_old_inheritance)
//{
//    PYTHON_MODULE_INSTALL(object_test);
//    PYTHON_MODULE_INSTALL(pluto_module);
//
//    PYTHON_TEST_PREPARE();
//    {
//
//        //TestClass::static_class()->ready();
//
//        //TestClass* obj = object_new<TestClass>();
//        //ASSERT_NO_PYTHON_ERROR();
//        //PyRun_SimpleString(
//        //    "import object_test as t\n"
//        //    "from pluto_module import register_class, install\n"
//        //    "@register_class\n"
//        //    "class A(t.TestClass):\n"
//        //    "    asd=[1,2,3]\n"
//        //    "    def __init__(self):\n"
//        //    "        super(A, self).__init__()\n"
//        //    "        self.a = 123\n"
//        //    "install(A())\n"
//        //);
//        //ASSERT_NO_PYTHON_ERROR();
//        //obj;
//
//    }
//    PYTHON_TEST_CLEANUP();
//}

