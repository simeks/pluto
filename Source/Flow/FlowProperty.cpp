#include <Core/Common.h>

#include "FlowProperty.h"


OBJECT_INIT_TYPE_FN(FlowProperty)
{
    OBJECT_PYTHON_NO_METHODS();
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
    _default_value = nullptr;
}
void FlowProperty::object_init(const char* name, const char* default_value)
{
    _owner = nullptr;
    _name = name;
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




namespace python_convert
{
    template<>
    FLOW_API PyObject* to_python<FileProperty::FileMode>(const FileProperty::FileMode& type)
    {
        return to_python((int)type);
    }
    template<>
    FLOW_API FileProperty::FileMode from_python<FileProperty::FileMode>(PyObject* obj)
    {
        return (FileProperty::FileMode)from_python<int>(obj);
    }
}

OBJECT_INIT_TYPE_FN(FileProperty)
{
    OBJECT_PYTHON_NO_METHODS();

    OBJECT_PYTHON_ADD_CLASS_ATTR("File_Open", FileProperty::File_Open);
    OBJECT_PYTHON_ADD_CLASS_ATTR("File_Save", FileProperty::File_Save);
}

IMPLEMENT_OBJECT(FileProperty, "FileProperty", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(FileProperty, FlowProperty);

FileProperty::~FileProperty()
{
}
void FileProperty::object_init()
{
    FlowProperty::object_init();
}
void FileProperty::object_python_init(const Tuple& args, const Dict&)
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
        _file_mode = python_convert::from_python<FileMode>(args.get(2));
    else
        _file_mode = File_Open;

    if (args.size() > 3)
        _file_filter = python_convert::from_python<std::string>(args.get(3));
    else
        _file_filter = "Files (*.*)";
}
FileProperty::FileMode FileProperty::file_mode() const
{
    return _file_mode;
}
const std::string& FileProperty::file_filter() const
{
    return _file_filter;
}
