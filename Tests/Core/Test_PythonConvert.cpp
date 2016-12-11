#include <Testing/Framework.h>

#include <Core/Image/ImageObject.h>
#include <Core/Object/Object.h>
#include <Core/Python/Convert.h>
#include <Core/Python/PythonType.h>

using namespace testing;

TEST_CASE(from_python)
{
    Py_Initialize();

    PyObject* long_obj = PyLong_FromLong(53);
    PyObject* float_obj = PyFloat_FromDouble(2.0);
    PyObject* str_obj = PyUnicode_FromString("test");

    ASSERT_EQUAL(python_convert::from_python<int>(long_obj), 53);
    ASSERT_EXPR(PyErr_Occurred() == nullptr);
    ASSERT_EQUAL(python_convert::from_python<uint32_t>(long_obj), 53);
    ASSERT_EXPR(PyErr_Occurred() == nullptr);
    ASSERT_EQUAL_F(python_convert::from_python<float>(long_obj), 53.0, FLT_EPSILON);
    ASSERT_EXPR(PyErr_Occurred() == nullptr);
    ASSERT_EQUAL_F(python_convert::from_python<double>(long_obj), 53.0, FLT_EPSILON);
    ASSERT_EXPR(PyErr_Occurred() == nullptr);
    ASSERT_EQUAL_F(python_convert::from_python<float>(float_obj), 2.0, FLT_EPSILON);
    ASSERT_EXPR(PyErr_Occurred() == nullptr);
    ASSERT_EQUAL_F(python_convert::from_python<double>(float_obj), 2.0, FLT_EPSILON);
    ASSERT_EXPR(PyErr_Occurred() == nullptr);
    ASSERT_EQUAL(python_convert::from_python<int>(float_obj), 2);
    ASSERT_EXPR(PyErr_Occurred() == nullptr);
    ASSERT_EQUAL(python_convert::from_python<uint32_t>(float_obj), 2);
    ASSERT_EXPR(PyErr_Occurred() == nullptr);
    ASSERT_EXPR(python_convert::from_python<std::string>(str_obj) == "test");
    ASSERT_EXPR(PyErr_Occurred() == nullptr);
}
TEST_CASE(from_python_object)
{
    Py_Initialize();
    PythonType::ready_all();
    numpy::initialize();

    Object* obj = new Object();
    ASSERT_EQUAL(python_convert::from_python<Object*>(obj->python_object()), obj);
    ASSERT_EXPR(PyErr_Occurred() == nullptr);
    ASSERT_NOT_EQUAL(python_convert::from_python<ImageObject*>(obj->python_object()), obj);
    PyErr_Print();
    ASSERT_EXPR(PyErr_Occurred() == nullptr);

    ImageObject* img = new ImageObject();
    ASSERT_EQUAL(python_convert::from_python<Object*>(img->python_object()), img);
    ASSERT_EXPR(PyErr_Occurred() == nullptr);
    ASSERT_EQUAL(python_convert::from_python<ImageObject*>(img->python_object()), img);
    ASSERT_EXPR(PyErr_Occurred() == nullptr);
}