#ifndef __CORE_PLATFORM_GUID_H__
#define __CORE_PLATFORM_GUID_H__

/// Represents a globally unique identifier
struct Guid
{
    Guid()
    {
        d1 = d2 = d3 = 0;
        for (int i = 0; i < 8; ++i)
            d4[i] = 0;
    }

    uint32_t d1;
    uint16_t d2;
    uint16_t d3;
    uint8_t d4[8];

    bool is_valid() const
    {
        if (d1 != 0 || d2 != 0 || d3 != 0)
            return true;

        for (int i = 0; i < 8; i++)
            if (d4[i] != 0)
                return true;

        return false;
    }
};

INLINE bool operator==(const Guid& x, const Guid& y)
{
    if (x.d1 != y.d1 || x.d2 != y.d2 || x.d3 != y.d3)
        return false;

    for (int i = 0; i < 8; ++i)
        if (x.d4[i] != y.d4[i])
            return false;

    return true;
}

INLINE bool operator!=(const Guid& x, const Guid& y)
{
    return !(x == y);
}

INLINE bool operator<(const Guid& x, const Guid& y)
{
    if (x.d1 < y.d1)
        return true;
    if (x.d1 > y.d1)
        return false;

    if (x.d2 < y.d2)
        return true;
    if (x.d2 > y.d2)
        return false;

    if (x.d3 < y.d3)
        return true;
    if (x.d3 > y.d3)
        return false;

    for (int i = 0; i < 8; ++i)
    {
        if (x.d4[i] < y.d4[i])
            return true;
        if (x.d4[i] > y.d4[i])
            return false;
    }

    return false;
}

namespace guid
{
    CORE_API Guid create_guid();

    /// Converts a GUID to a string of the format 00000000-0000-0000-0000-000000000000
    CORE_API std::string to_string(const Guid& id);

    /// Converts a string of the format 00000000-0000-0000-0000-000000000000 to a GUID
    CORE_API Guid from_string(const std::string& str);
}

#endif // __CORE_PLATFORM_GUID_H__
