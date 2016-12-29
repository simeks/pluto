#ifndef __PYTHON_COMMON_H__
#define __PYTHON_COMMON_H__

#include "PythonWrapper.h"
#include "Tuple.h"
#include "Dict.h"

#define PYTHON_ERROR(Exc, Text, ...) do{PyErr_Format(PyExc_##Exc, Text, __VA_ARGS__); return;}while(0)
#define PYTHON_ERROR_R(Exc, Return, Text, ...) do{PyErr_Format(PyExc_##Exc, Text, __VA_ARGS__); return Return;}while(0)


#endif // __PYTHON_COMMON_H__
