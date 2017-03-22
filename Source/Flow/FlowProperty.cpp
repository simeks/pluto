#include <Core/Common.h>
#include <Core/Python/Sequence.h>

#include "FlowProperty.h"


FlowProperty::FlowProperty(Type type) :
    type(type),
    owner(nullptr)
{
}
FlowProperty::~FlowProperty()
{
}

PrimitiveProperty::PrimitiveProperty() : 
    FlowProperty(Type_Primitive),
    default_value(python::None())
{
}
FileProperty::FileProperty() 
    : FlowProperty(Type_File)
{
}
EnumProperty::EnumProperty() 
    : FlowProperty(Type_Enum)
{

}



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


PYTHON_OBJECT_IMPL(FileProperty, "FileProperty")
{
    cls.def_init_varargs<FileProperty>();
    cls.def("File_Open", FileProperty::File_Open);
    cls.def("File_Save", FileProperty::File_Save);
}

FileProperty::FileProperty() : FlowProperty()
{
}
FileProperty::FileProperty(const python::Tuple& args) : FlowProperty(args)
{
    if (args.size() > 1)
    {
        _default_value = args.get(1);
    }

    if (args.size() > 2)
        _file_mode = python::from_python<FileMode>(args.get(2));
    else
        _file_mode = File_Open;

    if (args.size() > 3)
        _file_filter = python::from_python<std::string>(args.get(3));
    else
        _file_filter = "Files (*.*)";
}
FileProperty::~FileProperty()
{
}
FileProperty::FileMode FileProperty::file_mode() const
{
    return _file_mode;
}
const std::string& FileProperty::file_filter() const
{
    return _file_filter;
}


PYTHON_OBJECT_IMPL(EnumProperty, "EnumProperty")
{
    cls.def_init_varargs<EnumProperty>();
}

EnumProperty::EnumProperty() : FlowProperty()
{
}
EnumProperty::EnumProperty(const python::Tuple& args) : FlowProperty(args)
{
    if (args.size() < 2)
        PYTHON_ERROR(PyExc_ValueError, "Expected at least 2 arguments");

    _default_index = -1;

    python::Sequence opts(args.get(1));
    for (int i = 0; i < opts.size(); ++i)
    {
        python::Object str = opts.get(i);
        if (!PyUnicode_Check(str.ptr()))
            PYTHON_ERROR(PyExc_TypeError, "Expected all options to be strings");

        _options.push_back(PyUnicode_AsUTF8(str.ptr()));
    }

    if (args.size() > 2)
    {
        python::Object def = args.get(2);
        if (PyLong_Check(def.ptr()))
        {
            _default_index = python::from_python<int>(def);
            if (_default_index >= 0 && _default_index < opts.size())
            {
                _default_value = opts.get(_default_index);
            }
            else
            {
                PYTHON_ERROR(PyExc_TypeError, "Index out of bounds: 0 <= %d < %d", _default_index, opts.size());
            }
        }
        else
        {
            PYTHON_ERROR(PyExc_TypeError, "Expected default index to be int");
        }
    }
}
EnumProperty::~EnumProperty()
{
}
const std::vector<std::string>& EnumProperty::options() const
{
    return _options;
}
int EnumProperty::default_index() const
{
    return _default_index;
}
