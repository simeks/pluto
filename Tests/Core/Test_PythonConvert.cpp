#include <Testing/Framework.h>

#include <Core/Object/Object.h>
#include <Core/Object/PythonClass.h>
#include <Core/Python/Convert.h>
#include <Core/Python/NumPy.h>

using namespace testing;

TEST_CASE(from_python)
{
    Py_Initialize();
    {
        PyObject* long_obj = PyLong_FromLong(53);
        PyObject* float_obj = PyFloat_FromDouble(2.0);
        PyObject* str_obj = PyUnicode_FromString("test");

        ASSERT_EQUAL(python::from_python<int>(long_obj), 53);
        ASSERT_EXPR(PyErr_Occurred() == nullptr);
        ASSERT_EQUAL(python::from_python<uint32_t>(long_obj), 53);
        ASSERT_EXPR(PyErr_Occurred() == nullptr);
        ASSERT_EQUAL_F(python::from_python<float>(long_obj), 53.0, FLT_EPSILON);
        ASSERT_EXPR(PyErr_Occurred() == nullptr);
        ASSERT_EQUAL_F(python::from_python<double>(long_obj), 53.0, FLT_EPSILON);
        ASSERT_EXPR(PyErr_Occurred() == nullptr);
        ASSERT_EQUAL_F(python::from_python<float>(float_obj), 2.0, FLT_EPSILON);
        ASSERT_EXPR(PyErr_Occurred() == nullptr);
        ASSERT_EQUAL_F(python::from_python<double>(float_obj), 2.0, FLT_EPSILON);
        ASSERT_EXPR(PyErr_Occurred() == nullptr);
        ASSERT_EQUAL(python::from_python<int>(float_obj), 2);
        ASSERT_EXPR(PyErr_Occurred() == nullptr);
        ASSERT_EQUAL(python::from_python<uint32_t>(float_obj), 2);
        ASSERT_EXPR(PyErr_Occurred() == nullptr);
        ASSERT_EXPR(python::from_python<std::string>(str_obj) == "test");
        ASSERT_EXPR(PyErr_Occurred() == nullptr);
    }
    Py_Finalize();
}
TEST_CASE(from_python_object)
{
    Py_Initialize();
    {
        PythonClass::ready_all();
        numpy::initialize();

        Object* obj = object_new<Object>();
        ASSERT_EQUAL(python::from_python<Object*>(obj->python_object()), obj);
        ASSERT_EXPR(PyErr_Occurred() == nullptr);
    }
    Py_Finalize();
}