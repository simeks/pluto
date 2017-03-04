#ifndef __PYTHON_COMMON_H__
#define __PYTHON_COMMON_H__

#include <Core/Python/PythonWrapper.h>

#include <Core/Python/Convert.h>
#include <Core/Python/Dict.h>
#include <Core/Python/Error.h>
#include <Core/Python/Object.h>
#include <Core/Python/Sequence.h>
#include <Core/Python/Tuple.h>

using python::Dict;
using python::Sequence;
using python::Tuple;

#define PYTHON_STDOUT(Text, ...) PySys_FormatStdout(Text, __VA_ARGS__)
#define PYTHON_STDERR(Text, ...) PySys_FormatStderr(Text, __VA_ARGS__)

#endif // __PYTHON_COMMON_H__
