#include <Testing/Framework.h>

#include <Core/Image/Types.h>
#include <Core/Python/Numpy.h>
#include <Core/Python/PythonCommon.h>

#include "Test_Python.h"

using namespace testing;

namespace
{
    template<typename T>
    void fill_array(const NumpyArray& arr)
    {
        uint8_t* elems = (uint8_t*)arr.data();
        for (int y = 0; y < arr.shape()[0]; ++y)
        {
            for (int x = 0; x < arr.shape()[1]; ++x)
            {
                *((T*)(elems + y*arr.stride(0) + x*arr.stride(1))) = T(arr.shape()[0] * y + x);
            }
        }
    }

    template<typename T>
    bool check_array(const NumpyArray& arr)
    {
        uint8_t* elems = (uint8_t*)arr.data();
        for (int y = 0; y < arr.shape()[0]; ++y)
        {
            for (int x = 0; x < arr.shape()[1]; ++x)
            {
                if (*((T*)(elems + y*arr.stride(0) + x*arr.stride(1))) != T(arr.shape()[0] * y + x))
                {
                    return false;
                }
            }
        }
        return true;
    }

    template<>
    bool check_array<float>(const NumpyArray& arr)
    {
        uint8_t* elems = (uint8_t*)arr.data();
        for (int y = 0; y < arr.shape()[0]; ++y)
        {
            for (int x = 0; x < arr.shape()[1]; ++x)
            {
                if (fabs(*((float*)(elems + y*arr.stride(0) + x*arr.stride(1))) - float(arr.shape()[0] * y + x)) > FLT_EPSILON)
                {
                    return false;
                }
            }
        }
        return true;
    }

    void intialize_numpy()
    {
        static bool initialized = false;
        if (!initialized)
        {
            numpy::initialize();
            initialized = true;
        }
    }
}

TEST_CASE(ndarray)
{
    Py_Initialize();
    intialize_numpy();
    {
        Py_intptr_t dims[] = { 5, 10 };
        NumpyArray arr(2, dims, NPY_UINT32);
        ASSERT_NO_PYTHON_ERROR();

        ASSERT_EXPR(arr.valid());
        ASSERT_EQUAL(arr.ndims(), 2);
        ASSERT_EQUAL(arr.shape()[0], 5);
        ASSERT_EQUAL(arr.shape()[1], 10);
        ASSERT_EQUAL(arr.nelems(), 5 * 10);
        ASSERT_EQUAL(arr.element_size(), sizeof(uint32_t)); // NPY_UINT32
        ASSERT_EQUAL(arr.nbytes(), 5 * 10 * sizeof(uint32_t));

        fill_array<uint32_t>(arr);
        ASSERT_EXPR(check_array<uint32_t>(arr));
    }
    Py_Finalize();
}

TEST_CASE(ndarray_convert)
{
    Py_Initialize();
    intialize_numpy();
    {
        Py_intptr_t dims[] = { 5, 10 };
        NumpyArray arr(2, dims, NPY_UINT32);
        ASSERT_NO_PYTHON_ERROR();

        fill_array<uint32_t>(arr);
        python::Object pyobj = python::to_python(arr);
        ASSERT_NO_PYTHON_ERROR();

        ASSERT_EXPR(numpy::check_type(pyobj.ptr()));

        NumpyArray arr2 = python::from_python<NumpyArray>(pyobj);
        ASSERT_EXPR(check_array<uint32_t>(arr2));


        ASSERT_EXPR(arr2.valid());
        ASSERT_EQUAL(arr2.ndims(), 2);
        ASSERT_EQUAL(arr2.shape()[0], 5);
        ASSERT_EQUAL(arr2.shape()[1], 10);
        ASSERT_EQUAL(arr2.nelems(), 5 * 10);
        ASSERT_EQUAL(arr2.nbytes(), 5 * 10 * sizeof(uint32_t));
        ASSERT_EQUAL(arr2.element_size(), sizeof(uint32_t)); // NPY_UINT32
    }
    Py_Finalize();
}

TEST_CASE(ndarray_cast)
{
    Py_Initialize();
    intialize_numpy();
    {
        Py_intptr_t dims[] = { 2, 4 };
        NumpyArray arr(2, dims, NPY_UINT32);
        ASSERT_NO_PYTHON_ERROR();

        fill_array<uint32_t>(arr);

        NumpyArray arr_u8 = arr.cast(NPY_UINT8);
        ASSERT_NO_PYTHON_ERROR();

        ASSERT_EXPR(check_array<uint8_t>(arr_u8));

        NumpyArray arr_f32 = arr.cast(NPY_FLOAT32);
        ASSERT_NO_PYTHON_ERROR();

        ASSERT_EXPR(check_array<float>(arr_f32));

    }
    Py_Finalize();
}

