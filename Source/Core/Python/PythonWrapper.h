#ifndef __CORE_PYTHON_WRAPPER_H__
#define __CORE_PYTHON_WRAPPER_H__

#pragma warning(push)
#pragma warning(disable:4510) // warning C4510: '<unnamed-tag>' : default constructor could not be generated
#pragma warning(disable:4512) // warning C4512: '<unnamed-tag>' : assignment operator could not be generated
#pragma warning(disable:4610) // warning C4610: struct '<unnamed-tag>' can never be instantiated - user defined constructor required

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#pragma warning(pop)


#endif // __CORE_PYTHON_WRAPPER_H__
