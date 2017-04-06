#ifndef __FLOW_PROPERTY_H__
#define __FLOW_PROPERTY_H__

#include <Core/Object/Object.h>

#include "API.h"

class FlowNode;

class FLOW_API FlowProperty : public Object
{
    PLUTO_OBJECT(FlowProperty, Object);

public:
    FlowProperty();
    FlowProperty(const python::Object& value);
    virtual ~FlowProperty();

    const char* name() const;
    void set_name(const char* name);

    FlowNode* owner() const;
    void set_owner(FlowNode* owner);

    python::Object value() const;
    void set_value(const python::Object& value);

private:
    std::string _name;
    FlowNode* _owner;
    python::Object _value;
};

class FileProperty : public FlowProperty
{
    PLUTO_OBJECT(FileProperty, FlowProperty);

public:
    enum FileMode
    {
        File_Open,
        File_Save
    };

    FileProperty();
    FileProperty(const python::Tuple& args);

    FileMode file_mode() const;
    void set_file_mode(FileMode mode);

    const char* file_filter() const;
    void set_file_filter(const char* filter);

private:
    FileMode _file_mode;
    std::string _file_filter;
};

class EnumProperty : public FlowProperty
{
    PLUTO_OBJECT(EnumProperty, FlowProperty);

public:
    EnumProperty();
    EnumProperty(const python::Tuple& args);

    const std::vector<std::string>& options() const;
    int default_index() const;

private:
    std::vector<std::string> _options;
    int _default_index; // Index to default value
};



#endif // __FLOW_PROPERTY_H__
