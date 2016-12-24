#ifndef __PYTHON_COMMON_H__
#define __PYTHON_COMMON_H__

#include "PythonWrapper.h"
#include "Tuple.h"
#include "Dict.h"

#define PYTHON_ERROR(Exc, Text) do{PyErr_SetString(PyExc_##Exc, Text); return;}while(0)
#define PYTHON_ERROR_R(Exc, Text, Return) do{PyErr_SetString(PyExc_##Exc, Text); return Return;}while(0)


#endif // __PYTHON_COMMON_H__
