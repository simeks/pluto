#include <Testing/Framework.h>

#include <Core/Python/Convert.h>
#include <Core/Python/NumPy.h>

#include "Test_Python.h"

using namespace testing;

TEST_CASE(builtins_from_python)
{
    PYTHON_TEST_PREPARE();
    {
        python::Object long_obj = PyLong_FromLong(53);
        ASSERT_NO_PYTHON_ERROR();
        python::Object float_obj = PyFloat_FromDouble(2.0);
        ASSERT_NO_PYTHON_ERROR();
        python::Object str_obj = PyUnicode_FromString("test");
        ASSERT_NO_PYTHON_ERROR();

        ASSERT_EQUAL(python::from_python<int>(long_obj), 53);
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL(python::from_python<uint32_t>(long_obj), 53);
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL_F(python::from_python<float>(long_obj), 53.0, FLT_EPSILON);
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL_F(python::from_python<double>(long_obj), 53.0, FLT_EPSILON);
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL_F(python::from_python<float>(float_obj), 2.0, FLT_EPSILON);
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL_F(python::from_python<double>(float_obj), 2.0, FLT_EPSILON);
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL(python::from_python<int>(float_obj), 2);
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL(python::from_python<uint32_t>(float_obj), 2);
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EXPR(python::from_python<std::string>(str_obj) == "test");
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EQUAL_STR(python::from_python<const char*>(str_obj), "test");
        ASSERT_NO_PYTHON_ERROR();
    }
    PYTHON_TEST_CLEANUP();
}

TEST_CASE(builtins_to_python)
{
    PYTHON_TEST_PREPARE();
    {
        python::Object long_obj = python::to_python(53);
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EXPR(long_obj.is_instance(&PyLong_Type));
        ASSERT_EQUAL(PyLong_AsLong(long_obj.ptr()), 53);

        python::Object float_obj = python::to_python(2.0);
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EXPR(float_obj.is_instance(&PyFloat_Type));
        ASSERT_EQUAL_F(PyFloat_AsDouble(float_obj.ptr()), 2.0, FLT_EPSILON);

        python::Object str_obj = python::to_python("test");
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EXPR(str_obj.is_instance(&PyUnicode_Type));
        ASSERT_EQUAL_STR(PyUnicode_AsUTF8(str_obj.ptr()), "test");

        python::Object stdstr_obj = python::to_python(std::string("stdtest"));
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EXPR(stdstr_obj.is_instance(&PyUnicode_Type));
        ASSERT_EQUAL_STR(PyUnicode_AsUTF8(stdstr_obj.ptr()), "stdtest");
    }
    PYTHON_TEST_CLEANUP();
}

struct SomeType
{
    int value;
};

struct SomeTypeConverter
{
    static PyObject* to_python(const SomeType& value)
    {
        return PyLong_FromLong(value.value);
    }
    static SomeType from_python(PyObject* obj)
    {
        SomeType ret;
        ret.value = PyLong_AsLong(obj);
        return ret;
    }
};


TEST_CASE(python_custom_converter)
{
    PYTHON_TEST_PREPARE();
    python::TypeConverter<SomeType, SomeTypeConverter>();
    {
        SomeType value;
        value.value = 4342;

        python::Object obj = python::to_python(value);
        ASSERT_NO_PYTHON_ERROR();

        SomeType value2 = python::from_python<SomeType>(obj);
        ASSERT_NO_PYTHON_ERROR();

        ASSERT_EQUAL(value.value, value2.value);
    }
    PYTHON_TEST_CLEANUP();
}
