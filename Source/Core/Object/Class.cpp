#include "Common.h"

#include "Class.h"


Class::Class(const char* name, size_t size, CreateObjectFn creator)
    : Type(name, size),
    _creator(creator),
    _super(nullptr)
{
}

Class* Class::super_class() const
{
    return _super;
}
bool Class::is_class(Class* type) const
{
    for (const Class* t = this; t; t = t->super_class())
    {
        if (t == type)
            return true;
    }
    return false;
}
void Class::set_super(Class* super)
{
    _super = super;
}
Object* Class::create_object() const
{
    return _creator();
}
