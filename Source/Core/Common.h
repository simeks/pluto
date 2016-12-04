#ifndef __CORE_COMMON_H__
#define __CORE_COMMON_H__

#include <math.h>
#include <limits>
#include <iostream>
#include <algorithm>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <memory>
#include <type_traits>
#include <cstdarg>
#include <stdio.h>
#include <string.h>

#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <unordered_map>
#include <deque>

#include "Platform/Platform.h"
#include "Platform/Guid.h"
#include "API.h"
#include "Debug/Assert.h"
#include "Memory/Memory.h"
#include "Platform/StringConv.h"
#include "Util.h"
#include "NonCopyable.h"

#ifndef PyObject_HEAD
struct _object;
typedef _object PyObject;

struct _typeobject;
typedef _typeobject PyTypeObject;
#endif

#endif // __CORE_COMMON_H__
