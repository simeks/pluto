#ifndef __FLOW_PROPERTY_H__
#define __FLOW_PROPERTY_H__

#include <Core/Object/Object.h>

#include "API.h"

class FlowNode;
class FLOW_API FlowProperty : public Object
{
    DECLARE_OBJECT(FlowProperty, Object);

public:
    enum Type
    {
        Type_String,
        Type_Int,
        Type_Float,
        Type_Bool,
        Type_FilePath
    };

    enum FileMode
    {
        File_Open,
        File_Save
    };
    
    DECLARE_OBJECT_CONSTRUCTOR(FlowProperty);
    ~FlowProperty();

    void object_init();
    void object_init(const char* name, const char* default_value, Type type = Type_String);
    void object_python_init(const Tuple&, const Dict&);

    const char* name() const;
    PyObject* default_value() const;

    FlowNode* owner() const;
    void set_owner(FlowNode* node);

    Type type() const;

    /// File mode if property is of type FilePath
    FileMode file_mode() const;

    /// File filter if property is of type FilePath
    const std::string& file_filter() const;

private:
    std::string _name;
    Type _type;

    FileMode _file_mode;
    std::string _file_filter;

    PyObject* _default_value;

    FlowNode* _owner;
};

#endif // __FLOW_PROPERTY_H__
