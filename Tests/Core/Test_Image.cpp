#include <Testing/Framework.h>

#include <Core/Image/Types.h>
#include <Core/Python/Numpy.h>
#include <Core/Python/PythonCommon.h>

#include "Test_Python.h"

using namespace testing;


TEST_CASE(pixel_type)
{
    Py_Initialize();
    {
        image::PixelType ptype = image::PixelType_Vec3u8;

        python::Object ptype_py = python::to_python(ptype);
        ASSERT_NO_PYTHON_ERROR();
        ASSERT_EXPR(ptype_py.is_instance(&PyLong_Type));

        image::PixelType ptype2 = python::from_python<image::PixelType>(ptype_py);
        ASSERT_NO_PYTHON_ERROR();

        int ptype2_int = python::from_python<int>(ptype_py);
        ASSERT_NO_PYTHON_ERROR();

        ASSERT_EQUAL(ptype2, ptype);
        ASSERT_EQUAL(ptype2_int, (int)ptype);
    }
    Py_Finalize();
}


