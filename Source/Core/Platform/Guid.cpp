#include "Common.h"

#include "Guid.h"
#include "Timer.h"

#if defined(PLUTO_PLATFORM_WINDOWS)
#include <Objbase.h>

Guid guid::create_guid()
{
    Guid id;
    HRESULT hr = CoCreateGuid((GUID*)&id);
    assert(hr == S_OK);
    hr;

    return id;
}
#elif defined(PLUTO_PLATFORM_LINUX)
#include <sys/time.h>

Guid guid::create_guid()
{
    struct timeval t;
    gettimeofday(&t, NULL);

    struct tm lt;
    localtime_r(&t.tv_sec, &lt);

    uint32_t a = lt.tm_mday | (lt.tm_hour << 16);
    uint32_t b = (lt.tm_mon + 1) | (lt.tm_sec << 16);
    uint32_t c = (t.tv_usec / 1000) | (lt.tm_min << 16);
    uint32_t d = (lt.tm_year + 1900) ^ timer::tick_count();

    Guid id;
    id.d1 = a;
    id.d2 = b >> 16;
    id.d3 = b & 0xffff;

    int i = 0;
    for (; i < 4; ++i)
    {
        id.d4[i] = (c >> (8*(3-i))) & 0xff;
    }

    for (; i < 8; ++i)
    {
        id.d4[i] = (d >> (8*(7-i))) & 0xff;
    }

    return id;
}

#endif

std::string guid::to_string(const Guid& id)
{
    char str[40];
    sprintf(str, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", 
        id.d1, id.d2, id.d3, 
        id.d4[0], id.d4[1], id.d4[2], id.d4[3], id.d4[4], id.d4[5], id.d4[6], id.d4[7]);
    return str;
}
Guid guid::from_string(const std::string& str)
{
    assert(str.length() == 36);

    Guid id;

    uint32_t d[11];

    sscanf(str.c_str(), "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        &d[0], &d[1], &d[2],
        &d[3], &d[4], &d[5], &d[6], &d[7], &d[8], &d[9], &d[10]);

    id.d1 = d[0];
    id.d2 = d[1] & 0xFFFF;
    id.d3 = d[2] & 0xFFFF;

    for (int i = 3; i < 11; ++i)
        id.d4[i - 3] = d[i] & 0xFF;

    return id;
}
