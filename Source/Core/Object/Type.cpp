#include "Common.h"

#include "Type.h"


namespace object_types
{
    Type* g_head = nullptr;

    void register_type(Type* type)
    {
        if (g_head)
        {
            type->_list_next = g_head;
        }
        g_head = type;
    }
    void clear_types()
    {
        while (g_head)
        {
            Type* type = g_head;
            g_head = g_head->_list_next;
            delete type;
        }
    }
}


Type::Type(const char* name, uint32_t size)
    : _name(name),
    _size(size),
    _list_next(nullptr)
{
    object_types::register_type(this);
}

const char* Type::name() const
{
    return _name;
}

uint32_t Type::size() const
{
    return _size;
}
