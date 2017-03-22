#ifndef __FLOW_PROPERTY_H__
#define __FLOW_PROPERTY_H__

#include <Core/Python/BaseObject.h>

#include "API.h"

class FlowNode;

struct FlowProperty
{
    enum Type
    {
        Type_Primitive, /// I.e. int, float, string, bool
        Type_File,
        Type_Enum
    };

    FlowProperty(Type type);
    virtual ~FlowProperty();

    Type type;
    FlowNode* owner;
};

struct PrimitiveProperty : public FlowProperty
{
    PrimitiveProperty();

    python::Object default_value;
};

struct FileProperty : public FlowProperty
{
    enum FileMode
    {
        File_Open,
        File_Save
    };

    FileProperty();

    FileMode file_mode;
    std::string file_filter;
};

struct EnumProperty : public FlowProperty
{
    EnumProperty();

    std::vector<std::string> options;
    int default_index; // Index to default value
};



#endif // __FLOW_PROPERTY_H__
