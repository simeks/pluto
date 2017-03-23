#include <Core/Common.h>
#include <Core/Python/Sequence.h>

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

FlowProperty::FlowProperty(Type type) :
    type(type),
    owner(nullptr)
{
}
FlowProperty::~FlowProperty()
{
}
python::Class FlowProperty::python_class()
{
    static python::Class cls;
    if (cls.is_none())
    {
        cls = python::make_class<FlowProperty>("Property");
    }
    return cls;
}

PrimitiveProperty::PrimitiveProperty() : 
    FlowProperty(Type_Primitive),
    default_value(python::None())
{
}
PrimitiveProperty::PrimitiveProperty(const python::Object& default_value) :
    FlowProperty(Type_Primitive),
    default_value(default_value)
{
}
python::Class PrimitiveProperty::python_class()
{
    static python::Class cls;
    if (cls.is_none())
    {
        cls = python::make_class<PrimitiveProperty, FlowProperty>("PrimitiveProperty");
        cls.def_init<PrimitiveProperty, python::Object>();
    }
    return cls;
}

FileProperty::FileProperty() : FlowProperty(Type_File)
{
}
FileProperty::FileProperty(const python::Tuple& args) : FlowProperty(Type_File)
{
    if (args.size() > 1)
        default_value = args.get<const char*>(0);

    if (args.size() > 2)
        file_mode = python::from_python<FileMode>(args.get(1));
    else
        file_mode = File_Open;

    if (args.size() > 3)
        file_filter = python::from_python<std::string>(args.get(2));
    else
        file_filter = "Files (*.*)";
}
python::Class FileProperty::python_class()
{
    static python::Class cls;
    if (cls.is_none())
    {
        cls = python::make_class<FileProperty, FlowProperty>("FileProperty");
        cls.def_init_varargs<FileProperty>();
        cls.def("File_Open", FileProperty::File_Open);
        cls.def("File_Save", FileProperty::File_Save);
    }
    return cls;
}

EnumProperty::EnumProperty() : FlowProperty(Type_Enum)
{
}
EnumProperty::EnumProperty(const python::Tuple& args) : FlowProperty(Type_Enum)
{
    if (args.size() < 1)
        PYTHON_ERROR(PyExc_ValueError, "Expected at least 1 arguments");

    default_index = -1;

    python::Sequence opts(args.get(0));
    for (int i = 0; i < opts.size(); ++i)
    {
        python::Object str = opts.get(i);
        if (!PyUnicode_Check(str.ptr()))
            PYTHON_ERROR(PyExc_TypeError, "Expected all options to be strings");

        options.push_back(PyUnicode_AsUTF8(str.ptr()));
    }

    if (args.size() > 2)
    {
        python::Object def = args.get(1);
        if (PyLong_Check(def.ptr()))
        {
            default_index = python::from_python<int>(def);
            if (default_index < 0 && default_index >= opts.size())
            {
                PYTHON_ERROR(PyExc_TypeError, "Index out of bounds: 0 <= %d < %d", default_index, opts.size());
            }
        }
        else
        {
            PYTHON_ERROR(PyExc_TypeError, "Expected default index to be int");
        }
    }
}
python::Class EnumProperty::python_class()
{
    static python::Class cls;
    if (cls.is_none())
    {
        cls = python::make_class<EnumProperty, FlowProperty>("FileProperty");
        cls.def_init_varargs<EnumProperty>();
    }
    return cls;
}

