#ifndef __TEST_PYTHON_H__
#define __TEST_PYTHON_H__

#include <Testing/Framework.h>

#define ASSERT_NO_PYTHON_ERROR() \
    do{if (PyErr_Occurred()) \
    { \
        PyErr_Print(); \
        ASSERT_EXPR(false && "Python Error"); \
    }}while(0);


#define PYTHON_TEST_PREPARE() \
    python::type_registry::clear(); \
    Py_Initialize()

#define PYTHON_TEST_CLEANUP() \
    Py_Finalize()

#endif // __TEST_PYTHON_H__
