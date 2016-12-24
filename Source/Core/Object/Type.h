#ifndef __CORE_OBJECT_TYPE_H__
#define __CORE_OBJECT_TYPE_H__

#include <Core/Common.h>

class Type;

namespace object_types
{
    CORE_API void register_type(Type* type);

    /// Clears all registered types. 
    /// Should only be called during application shut down.
    void clear_types();
}

class CORE_API Type
{
public:
    Type(const char* name, size_t size);

    const char* name() const;
    size_t size() const;

    friend void object_types::register_type(Type* type);
    friend void object_types::clear_types();
private:
    const char* _name;
    size_t _size;

    Type* _list_next;
};

#endif // __CORE_OBJECT_TYPE_H__
