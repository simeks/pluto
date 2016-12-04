#include "Common.h"
#include "Platform/WindowsWrapper.h"

#include "../Platform.h"

#include <io.h>
#include <fcntl.h>

void platform::set_utf8_output()
{
    _setmode(_fileno(stdout), _O_U8TEXT);
}
