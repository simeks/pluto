#ifndef __CORE_OBJECT_CLASS_H__
#define __CORE_OBJECT_CLASS_H__

#include "Type.h"

class Object;
class CORE_API Class : public Type
{
public:
    Class(const char* name, size_t size);

    Class* super_class() const;
    void set_super(Class* super);

    // Checks if this class is either c or a subclass of c
    bool is_class(Class* c) const;

private:
    Class* _super; // Super class
};

#endif // __CORE_OBJECT_CLASS_H__
