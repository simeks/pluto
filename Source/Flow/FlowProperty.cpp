#include <Core/Common.h>

#include "FlowProperty.h"


namespace python_convert
{
    template<>
    FLOW_API PyObject* to_python<FlowProperty::Type>(const FlowProperty::Type& type)
    {
        return to_python((int)type);
    }
    template<>
    FLOW_API FlowProperty::Type from_python<FlowProperty::Type>(PyObject* obj)
    {
        return (FlowProperty::Type)from_python<int>(obj);
    }
    template<>
    FLOW_API PyObject* to_python<FlowProperty::FileMode>(const FlowProperty::FileMode& type)
    {
        return to_python((int)type);
    }
    template<>
    FLOW_API FlowProperty::FileMode from_python<FlowProperty::FileMode>(PyObject* obj)
    {
        return (FlowProperty::FileMode)from_python<int>(obj);
    }
}

OBJECT_INIT_TYPE_FN(FlowProperty)
{
    OBJECT_PYTHON_NO_METHODS();

    OBJECT_PYTHON_ADD_CLASS_ATTR("Type_String", FlowProperty::Type_String);
    OBJECT_PYTHON_ADD_CLASS_ATTR("Type_Int", FlowProperty::Type_Int);
    OBJECT_PYTHON_ADD_CLASS_ATTR("Type_Float", FlowProperty::Type_Float);
    OBJECT_PYTHON_ADD_CLASS_ATTR("Type_Bool", FlowProperty::Type_Bool);
    OBJECT_PYTHON_ADD_CLASS_ATTR("Type_FilePath", FlowProperty::Type_FilePath);

    OBJECT_PYTHON_ADD_CLASS_ATTR("File_Open", FlowProperty::File_Open);
    OBJECT_PYTHON_ADD_CLASS_ATTR("File_Save", FlowProperty::File_Save);
}

IMPLEMENT_OBJECT(FlowProperty, "FlowProperty", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(FlowProperty, Object);

FlowProperty::~FlowProperty()
{
    Py_XDECREF(_default_value);
}
void FlowProperty::object_init()
{
    _owner = nullptr;
    _type = Type_String;
    _default_value = nullptr;
}
void FlowProperty::object_init(const char* name, const char* default_value, Type type)
{
    _owner = nullptr;
    _name = name;
    _type = type;
    _default_value = PyUnicode_FromString(default_value);
}
void FlowProperty::object_python_init(const Tuple& args, const Dict&)
{
    _owner = nullptr;
    if (args.size() > 0)
        _name = python_convert::from_python<std::string>(args.get(0));

    if (args.size() > 1)
    {
        _default_value = args.get(1);
        Py_XINCREF(_default_value);
    }

    if (args.size() > 2)
        _type = python_convert::from_python<Type>(args.get(2));
    else
        _type = Type_String; 

    if (_type == Type_FilePath)
    {
        if (args.size() > 3)
            _file_mode = python_convert::from_python<FileMode>(args.get(3));
        else
            _file_mode = File_Open;

        if (args.size() > 4)
            _file_filter = python_convert::from_python<std::string>(args.get(4));
        else
            _file_filter = "Files (*.*)";
    }
}
const char* FlowProperty::name() const
{
    return _name.c_str();
}
PyObject* FlowProperty::default_value() const
{
    return _default_value;
}
FlowNode* FlowProperty::owner() const
{
    return _owner;
}
void FlowProperty::set_owner(FlowNode* node)
{
    _owner = node;
}
FlowProperty::Type FlowProperty::type() const
{
    return _type;
}
FlowProperty::FileMode FlowProperty::file_mode() const
{
    return _file_mode;
}
const std::string& FlowProperty::file_filter() const
{
    return _file_filter;
}
