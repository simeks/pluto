#include <Core/Common.h>
#include <Core/Python/Sequence.h>
#include <Core/Python/Str.h>

#include "FlowProperty.h"


namespace python
{
    template<>
    FLOW_API PyObject* to_python(const FileProperty::FileMode& type)
    {
        return to_python((int)type);
    }
    template<>
    FLOW_API FileProperty::FileMode from_python(const python::Object& obj)
    {
        return (FileProperty::FileMode)from_python<int>(obj);
    }
}

PLUTO_OBJECT_IMPL(FlowProperty, "Property")
{
    cls.def_init<FlowProperty, python::Object>();
    cls.def("value", &FlowProperty::value, "Returns the current value of this property");
    cls.def("set_value", &FlowProperty::set_value, "Sets the value of this property");
}

FlowProperty::FlowProperty() :
    _owner(nullptr),
    _value(python::None())
{
}
FlowProperty::FlowProperty(const python::Object& value) :
    _owner(nullptr),
    _value(value)
{
}
FlowProperty::~FlowProperty()
{
}
const char* FlowProperty::name() const
{
    return _name.c_str();
}
void FlowProperty::set_name(const char* name)
{
    _name = name;
}
FlowNode* FlowProperty::owner() const
{
    return _owner;
}
void FlowProperty::set_owner(FlowNode* owner)
{
    _owner = owner;
}
python::Object FlowProperty::value() const
{
    return _value;
}
void FlowProperty::set_value(const python::Object& value)
{
    _value = value;
}


PLUTO_OBJECT_IMPL(FileProperty, "FileProperty")
{
    cls.def_init_varargs<FileProperty>();
    cls.def("File_Open", FileProperty::File_Open);
    cls.def("File_Save", FileProperty::File_Save);
}

FileProperty::FileProperty() : FlowProperty(python::Str()),
    _file_mode(File_Open),
    _file_filter("Files (*.*)")
{
}
FileProperty::FileProperty(const python::Tuple& args) : 
    FlowProperty(python::Str()),
    _file_mode(File_Open),
    _file_filter("Files (*.*)")
{
    if (args.size() > 1)
    {
        if (!PyUnicode_Check(args.get(0).ptr()))
        {
            PYTHON_ERROR(PyExc_ValueError, "Expected value to be string");
        }
        set_value(args.get(0));
    }

    if (args.size() > 2)
    {
        _file_mode = python::from_python<FileMode>(args.get(1));
    }

    if (args.size() > 3)
    {
        _file_filter = python::from_python<std::string>(args.get(2));
    }
}
FileProperty::FileMode FileProperty::file_mode() const
{
    return _file_mode;
}
void FileProperty::set_file_mode(FileMode mode)
{
    _file_mode = mode;
}
const char* FileProperty::file_filter() const
{
    return _file_filter.c_str();
}
void FileProperty::set_file_filter(const char* filter)
{
    _file_filter = filter;
}

PLUTO_OBJECT_IMPL(EnumProperty, "EnumProperty")
{
    cls.def_init_varargs<EnumProperty>();
}
EnumProperty::EnumProperty() : FlowProperty(python::Str())
{
}
EnumProperty::EnumProperty(const python::Tuple& args) : 
    FlowProperty(python::Str()),
    _default_index(-1)
{
    if (args.size() < 1)
        PYTHON_ERROR(PyExc_ValueError, "Expected at least 1 arguments");

    python::Sequence opts(args.get(0));
    for (int i = 0; i < opts.size(); ++i)
    {
        python::Object str = opts.get(i);
        if (!PyUnicode_Check(str.ptr()))
            PYTHON_ERROR(PyExc_TypeError, "Expected all options to be strings");

        _options.push_back(PyUnicode_AsUTF8(str.ptr()));
    }

    if (args.size() > 2)
    {
        python::Object def = args.get(1);
        if (PyLong_Check(def.ptr()))
        {
            _default_index = python::from_python<int>(def);
            if (_default_index < 0 && _default_index >= opts.size())
            {
                PYTHON_ERROR(PyExc_TypeError, "Index out of bounds: 0 <= %d < %d", _default_index, opts.size());
            }
            set_value(opts.get(_default_index));
        }
        else
        {
            PYTHON_ERROR(PyExc_TypeError, "Expected default index to be int");
        }
    }
}
const std::vector<std::string>& EnumProperty::options() const
{
    return _options;
}
int EnumProperty::default_index() const
{
    return _default_index;
}

