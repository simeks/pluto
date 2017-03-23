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

    static python::Class python_class();
};

struct PrimitiveProperty : public FlowProperty
{
    PrimitiveProperty();
    PrimitiveProperty(const python::Object& default_value);

    python::Object default_value;

    static python::Class python_class();
};

struct FileProperty : public FlowProperty
{
    enum FileMode
    {
        File_Open,
        File_Save
    };

    FileProperty();
    FileProperty(const python::Tuple& args);

    std::string default_value;

    FileMode file_mode;
    std::string file_filter;

    static python::Class python_class();
};

struct EnumProperty : public FlowProperty
{
    EnumProperty();
    EnumProperty(const python::Tuple& args);

    std::vector<std::string> options;
    int default_index; // Index to default value

    static python::Class python_class();
};



#endif // __FLOW_PROPERTY_H__
