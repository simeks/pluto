#include <Testing/Framework.h>

#include <Core/Python/BaseObject.h>
#include <Core/Python/Class.h>
#include <Core/Python/Dict.h>
#include <Core/Python/Function.h>
#include <Core/Python/Module.h>
#include <Core/Python/NumPy.h>
#include <Core/Python/Object.h>

#include "Test_Python.h"

#define PYTHON_OBJECT(cls, super)

class Ball : public python::BaseObject
{
    PYTHON_OBJECT(Ball, python::BaseObject);

public:
    Ball() {}
    virtual ~Ball() {}

    virtual int radius()
    {
        return 0;
        /*python::Object method = attr("radius");
        if (is_overriden(method))
        {
            method();
        }*/
    }
};

class CppBall : public Ball
{
public:
    CppBall() {}
    virtual ~CppBall() {}

    int radius()
    {
        return 5;
    }
};


PYTHON_MODULE(py_test_base_object)
{
    python::Class cls = python::make_class<Ball>("Ball");
    cls.def_init<Ball>();
    
    module.def("Ball", cls);

    python::make_class<CppBall, Ball>("CppBall");
}



TEST_CASE(python_base_object)
{
    PYTHON_MODULE_INSTALL(py_test_base_object);
    PYTHON_TEST_PREPARE();
    {
        python::Object main = python::import("__main__");
        python::Object module = python::import("py_test_base_object");
        
        python::Dict d(python::Borrowed(PyModule_GetDict(main.ptr())));
        PyRun_String(
            "import py_test_base_object as p\n"
            "class PyBall(p.Ball):\n"
            "    def radius():\n"
            "        return 10\n"
            "\n"
            , Py_file_input, d.ptr(), nullptr);
        ASSERT_NO_PYTHON_ERROR();

        python::Object PyBall = d.get("PyBall");

        python::Ref<Ball> cpp_ball = python::make_object<CppBall>();
        python::Ref<Ball> py_ball = python::from_python<Ball*>(python::incref(PyBall().ptr()));


        ASSERT_EQUAL(cpp_ball->radius(), 5);
        ASSERT_EQUAL(py_ball->radius(), 10);

    }
    PYTHON_TEST_CLEANUP();
}
