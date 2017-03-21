#ifndef __FLOW_PROPERTY_H__
#define __FLOW_PROPERTY_H__

#include <Core/Python/BaseObject.h>

#include "API.h"

class FlowNode;
class FLOW_API FlowProperty : public python::BaseObject
{
    PYTHON_OBJECT(FlowProperty, python::BaseObject);

public:
    FlowProperty();
    FlowProperty(const char* name);
    FlowProperty(const python::Tuple& args);
    ~FlowProperty();

    const char* name() const;
    python::Object default_value() const;

    FlowNode* owner() const;
    void set_owner(FlowNode* node);

    FlowProperty(const FlowProperty& other);

protected:
    std::string _name;
    python::Object _default_value;

    FlowNode* _owner;
};

class FLOW_API FileProperty : public FlowProperty
{
    PYTHON_OBJECT(FileProperty, FlowProperty);
public:
    enum FileMode
    {
        File_Open,
        File_Save
    };

    FileProperty();
    FileProperty(const python::Tuple& args);
    ~FileProperty();

    /// File mode if property is of type FilePath
    FileMode file_mode() const;

    /// File filter if property is of type FilePath
    const std::string& file_filter() const;

protected:
    FileMode _file_mode;
    std::string _file_filter;
};

class FLOW_API EnumProperty : public FlowProperty
{
    PYTHON_OBJECT(EnumProperty, FlowProperty);
public:
    EnumProperty();
    EnumProperty(const python::Tuple& args);
    ~EnumProperty();

    const std::vector<std::string>& options() const;
    int default_index() const;

private:
    std::vector<std::string> _options;
    int _default_index; // Index to default value
};




#endif // __FLOW_PROPERTY_H__
