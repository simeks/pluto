// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Assert.h"

#ifdef PLUTO_PLATFORM_WINDOWS
#include "Platform/WindowsWrapper.h"
#endif

namespace assert_util
{

    AssertRetval util_assert_msg(const char* msg, const char* file_name, unsigned int line)
    {

#ifdef PLUTO_PLATFORM_WINDOWS
        char tmp[4096];

        sprintf_s(tmp, 4096, "%s:%u : %s", file_name, line, msg);

        //console::assert_message(tmp);

        wchar_t wtmp[4096];
        convert_string(tmp, wtmp, 4096);
        int res = MessageBox(NULL, wtmp, TEXT("Error"), MB_ABORTRETRYIGNORE | MB_ICONERROR);

        switch (res)
        {
        case IDRETRY:
            return RET_DEBUGGER;
        case IDIGNORE:
            return RET_CONTINUE;
        default:
            //case IDABORT:
            exit(0);
        };

#else
        return RET_DEBUGGER; // TODO:
#endif

    }

} 

