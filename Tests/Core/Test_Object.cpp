#include <Testing/Framework.h>

#include <Core/Object/Object.h>
#include <Core/Python/Object.h>
#include <Core/Python/Class.h>
#include <Core/Python/Dict.h>
#include <Core/Python/Function.h>
#include <Core/Python/Module.h>
#include <Core/Python/NumPy.h>

#include "Test_Python.h"


class Ball : public Object
{
	PLUTO_OBJECT(Ball, Object);
public:
    Ball() {}
    virtual ~Ball() {}

    virtual int radius()
    {
        python::Object method = attribute("radius");
        if (is_overridden(method))
        {
            return python::from_python<int>(method());
        }
        return -1;
    }
};
PLUTO_OBJECT_IMPL(Ball, "Ball")
{
	cls.def_init<Ball>();
}


class CppBall : public Ball
{
	PLUTO_OBJECT(CppBall, Ball);

public:
    CppBall() {}
    virtual ~CppBall() {}

    int radius()
    {
        return 5;
    }
};

PLUTO_OBJECT_IMPL(CppBall, "CppBall")
{
	cls;
}



PYTHON_MODULE(py_test_base_object)
{
    module.def("Ball", Ball::static_class());
}



TEST_CASE(pluto_object)
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
            "    def radius(self):\n"
            "        return 10\n"
            "\n"
            , Py_file_input, d.ptr(), nullptr);
        ASSERT_NO_PYTHON_ERROR();

        python::Object PyBall = d.get("PyBall");

        ObjectPtr<Ball> cpp_ball = make_object_ptr<CppBall>();
        // TODO: Handle reference count when converting to ObjectPtr<>
        ObjectPtr<Ball> py_ball = python::from_python<Ball*>(python::incref(PyBall().ptr()));

        ASSERT_EQUAL(cpp_ball->radius(), 5);
        ASSERT_EQUAL(py_ball->radius(), 10);
    }
    PYTHON_TEST_CLEANUP();
}

TEST_CASE(pluto_object_convert)
{
    PYTHON_MODULE_INSTALL(py_test_base_object);
    PYTHON_TEST_PREPARE();
    {
        python::import("py_test_base_object");

        ObjectPtr<CppBall> ball = make_object_ptr<CppBall>();
        python::Object pyobj = python::to_python(ball.ptr());
        ObjectPtr<CppBall> ball2 = python::from_python<CppBall*>(pyobj);

        // Object should reference both the same PyObject and C++ object at all times.

        ASSERT_EQUAL(ball.ptr(), ball2.ptr()); // C++ ptr
        ASSERT_EQUAL(ball->ptr(), ball2->ptr()); // PyObject*
        ASSERT_EQUAL(ball->ptr(), pyobj.ptr()); // PyObject*
    }
    PYTHON_TEST_CLEANUP();
}

class CopyClass : public Object
{
    PLUTO_OBJECT(CopyClass, Object);

public:
    CopyClass(int value) : _cpp_value(value) {}
    ~CopyClass() {}

    int _cpp_value;
};

PLUTO_OBJECT_IMPL(CopyClass, "CopyClass")
{
    cls;
}

TEST_CASE(pluto_object_copy)
{
    PYTHON_TEST_PREPARE();
    {
        // Initialize class
        CopyClass::static_class();
        
        ObjectPtr<CopyClass> obj = make_object_ptr<CopyClass>(456);
        obj->set_attribute("py_value", 321);
        ASSERT_EQUAL(obj->_cpp_value, 456);
        ASSERT_EQUAL(obj->attribute<int>("py_value"), 321);

        ObjectPtr<CopyClass> copy = clone_object(obj.ptr());
        ASSERT_EQUAL(copy->_cpp_value, 456);
        ASSERT_EQUAL(copy->attribute<int>("py_value"), 321);

    }
    PYTHON_TEST_CLEANUP();
}

class PtrClass : public Object
{
    PLUTO_OBJECT(PtrClass, Object);

public:
    PtrClass(int value) : _cpp_value(value) {}
    ~PtrClass() { destroyed = true; }

    int _cpp_value;

    static bool destroyed;
};

bool PtrClass::destroyed = false;

PLUTO_OBJECT_IMPL(PtrClass, "PtrClass")
{
    cls;
}


TEST_CASE(pluto_object_ptr)
{
    PYTHON_TEST_PREPARE();
    {
        // Initialize class
        PtrClass::static_class();

        ObjectPtr<PtrClass> obj = make_object_ptr<PtrClass>(123);
        ASSERT_EQUAL(obj->refcnt(), 1);

        ObjectPtr<PtrClass> obj2 = obj;
        ASSERT_EQUAL(obj->refcnt(), 2);

        python::Object pyobj = python::to_python(obj);
        ASSERT_EQUAL(obj->refcnt(), 3);

        ObjectPtr<PtrClass> obj_frompy = python::from_python<ObjectPtr<PtrClass>>(pyobj);
        ASSERT_EQUAL(obj->refcnt(), 4);

        obj2 = nullptr;
        ASSERT_EQUAL(obj->refcnt(), 3);

        pyobj = python::None();
        ASSERT_EQUAL(obj->refcnt(), 2);
    }
    ASSERT_EXPR(PtrClass::destroyed);

    PYTHON_TEST_CLEANUP();
}
