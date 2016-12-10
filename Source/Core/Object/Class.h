#ifndef __CORE_OBJECT_CLASS_H__
#define __CORE_OBJECT_CLASS_H__

#include "Type.h"

class Object;
class CORE_API Class : public Type
{
public:
    typedef Object* (*CreateObjectFn)();

    Class(const char* name, uint32_t size, CreateObjectFn creator);

    Object* create_object() const;

    Class* super_class() const;
    void set_super(Class* super);

    bool is_class(Class* c) const;

private:
    CreateObjectFn _creator;
    Class* _super; // Super class
};

#endif // __CORE_OBJECT_CLASS_H__
