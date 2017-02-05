#ifndef __FLOW_PROPERTY_H__
#define __FLOW_PROPERTY_H__

#include <Core/Object/Object.h>

#include "API.h"

class FlowNode;
class FLOW_API FlowProperty : public Object
{
    DECLARE_OBJECT(FlowProperty, Object);

public:
    DECLARE_OBJECT_CONSTRUCTOR(FlowProperty);
    ~FlowProperty();

    void object_init();
    void object_init(const char* name, const char* default_value);
    void object_python_init(const Tuple&, const Dict&);

    const char* name() const;
    PyObject* default_value() const;

    FlowNode* owner() const;
    void set_owner(FlowNode* node);

protected:
    std::string _name;
    PyObject* _default_value;

    FlowNode* _owner;
};

class FLOW_API FileProperty : public FlowProperty
{
    DECLARE_OBJECT(FileProperty, FlowProperty);
public:
    enum FileMode
    {
        File_Open,
        File_Save
    };

    DECLARE_OBJECT_CONSTRUCTOR(FileProperty);
    ~FileProperty();

    void object_init();
    void object_python_init(const Tuple&, const Dict&);

    /// File mode if property is of type FilePath
    FileMode file_mode() const;

    /// File filter if property is of type FilePath
    const std::string& file_filter() const;

protected:
    FileMode _file_mode;
    std::string _file_filter;
};


class FLOW_API BoolProperty : public FlowProperty
{
    DECLARE_OBJECT(BoolProperty, FlowProperty);
public:
    DECLARE_OBJECT_CONSTRUCTOR(BoolProperty);
    ~BoolProperty();

    void object_init();
    void object_python_init(const Tuple&, const Dict&);
}; 
class FLOW_API IntProperty : public FlowProperty
{
    DECLARE_OBJECT(IntProperty, FlowProperty);
public:
    DECLARE_OBJECT_CONSTRUCTOR(IntProperty);
    ~IntProperty();

    void object_init();
    void object_python_init(const Tuple&, const Dict&);
};
class FLOW_API FloatProperty : public FlowProperty
{
    DECLARE_OBJECT(FloatProperty, FlowProperty);
public:
    DECLARE_OBJECT_CONSTRUCTOR(FloatProperty);
    ~FloatProperty();

    void object_init();
    void object_python_init(const Tuple&, const Dict&);
};
class FLOW_API EnumProperty : public FlowProperty
{
    DECLARE_OBJECT(EnumProperty, FlowProperty);
public:
    DECLARE_OBJECT_CONSTRUCTOR(EnumProperty);
    ~EnumProperty();

    void object_init();
    void object_python_init(const Tuple&, const Dict&);

private:

};




#endif // __FLOW_PROPERTY_H__
