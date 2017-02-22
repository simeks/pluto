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
void FlowProperty::object_init(const char* name)
{
    _owner = nullptr;
    _default_value = nullptr;
    _name = name;
}
void FlowProperty::object_python_init(const Tuple& args, const Dict&)
{
    _owner = nullptr;
    if (args.size() > 0)
        _name = python::from_python<std::string>(args.get(0));
    _default_value = nullptr;
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
FlowProperty::FlowProperty(const FlowProperty& other) : Object(other)
{
    _name = other._name;
    _default_value = other._default_value;
    Py_XINCREF(_default_value);
    _owner = other._owner;
}


namespace python
{
    template<>
    FLOW_API python::Object to_python(const FileProperty::FileMode& type)
    {
        return to_python((int)type);
    }
    template<>
    FLOW_API FileProperty::FileMode from_python(const python::Object& obj)
    {
        return (FileProperty::FileMode)from_python<int>(obj.ptr());
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
void FileProperty::object_python_init(const Tuple& args, const Dict& kw)
{
    FlowProperty::object_python_init(args, kw);

    if (args.size() > 1)
    {
        _default_value = args.get(1);
        Py_XINCREF(_default_value);
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
FileProperty::FileMode FileProperty::file_mode() const
{
    return _file_mode;
}
const std::string& FileProperty::file_filter() const
{
    return _file_filter;
}


OBJECT_INIT_TYPE_FN(BoolProperty)
{
    OBJECT_PYTHON_NO_METHODS();
}

IMPLEMENT_OBJECT(BoolProperty, "BoolProperty", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(BoolProperty, FlowProperty);

BoolProperty::~BoolProperty()
{
}
void BoolProperty::object_init()
{
    FlowProperty::object_init();
}
void BoolProperty::object_init(const char* name, bool default_value)
{
    FlowProperty::object_init(name);
    _default_value = PyBool_FromLong(default_value);
}
void BoolProperty::object_python_init(const Tuple& args, const Dict& kw)
{
    FlowProperty::object_python_init(args, kw);

    _default_value = nullptr;
    if (args.size() > 1)
    {
        PyObject* def = args.get(1);
        if (PyBool_Check(def))
        {
            _default_value = def;
            Py_XINCREF(_default_value);
        }
        else
        {
            PYTHON_ERROR(TypeError, "Expected default argument to be bool");
        }
    }
}


OBJECT_INIT_TYPE_FN(IntProperty)
{
    OBJECT_PYTHON_NO_METHODS();
}

IMPLEMENT_OBJECT(IntProperty, "IntProperty", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(IntProperty, FlowProperty);

IntProperty::~IntProperty()
{
}
void IntProperty::object_init()
{
    FlowProperty::object_init();
}
void IntProperty::object_python_init(const Tuple& args, const Dict& kw)
{
    FlowProperty::object_python_init(args, kw);

    if (args.size() > 1)
    {
        PyObject* def = args.get(1);
        if (PyLong_Check(def))
        {
            _default_value = args.get(1);
            Py_XINCREF(_default_value);
        }
        else
        {
            PYTHON_ERROR(TypeError, "Expected default argument to be integer");
        }
    }
    else
        _default_value = PyLong_FromLong(0);
}


OBJECT_INIT_TYPE_FN(FloatProperty)
{
    OBJECT_PYTHON_NO_METHODS();
}

IMPLEMENT_OBJECT(FloatProperty, "FloatProperty", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(FloatProperty, FlowProperty);

FloatProperty::~FloatProperty()
{
}
void FloatProperty::object_init()
{
    FlowProperty::object_init();
}
void FloatProperty::object_python_init(const Tuple& args, const Dict& kw)
{
    FlowProperty::object_python_init(args, kw);

    if (args.size() > 1)
    {
        PyObject* def = args.get(1);
        if (PyNumber_Check(def))
        {
            _default_value = args.get(1);
            Py_XINCREF(_default_value);
        }
        else
        {
            PYTHON_ERROR(TypeError, "Expected default argument to be float");
        }
    }
}


OBJECT_INIT_TYPE_FN(EnumProperty)
{
    OBJECT_PYTHON_NO_METHODS();
}

IMPLEMENT_OBJECT(EnumProperty, "EnumProperty", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(EnumProperty, FlowProperty);

EnumProperty::~EnumProperty()
{
}
void EnumProperty::object_init()
{
    FlowProperty::object_init();
}
void EnumProperty::object_python_init(const Tuple& args, const Dict& kw)
{
    FlowProperty::object_python_init(args, kw);

    if (args.size() < 2)
        PYTHON_ERROR(ValueError, "Expected at least 2 arguments");

    _default_index = -1;

    Sequence opts(args.get(1));
    for (int i = 0; i < opts.size(); ++i)
    {
        PyObject* str = opts.get(i);
        if (!PyUnicode_Check(str))
            PYTHON_ERROR(TypeError, "Expected all options to be strings");

        _options.push_back(PyUnicode_AsUTF8(str));
    }

    if (args.size() > 2)
    {
        PyObject* def = args.get(2);
        if (PyLong_Check(def))
        {
            _default_index = python::from_python<int>(def);
            if (_default_index >= 0 && _default_index < opts.size())
            {
                _default_value = opts.get(_default_index);
                Py_XINCREF(_default_value);
            }
            else
            {
                PYTHON_ERROR(TypeError, "Index out of bounds: 0 <= %d < %d", _default_index, opts.size());
            }
        }
        else
        {
            PYTHON_ERROR(TypeError, "Expected default index to be int");
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

OBJECT_INIT_TYPE_FN(StringProperty)
{
    OBJECT_PYTHON_NO_METHODS();
}

IMPLEMENT_OBJECT(StringProperty, "StringProperty", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(StringProperty, FlowProperty);

StringProperty::~StringProperty()
{
}
void StringProperty::object_init()
{
    FlowProperty::object_init();
}
void StringProperty::object_init(const char* name, const char* default_value)
{
    FlowProperty::object_init(name);
    _default_value = PyUnicode_FromString(default_value);
}
void StringProperty::object_python_init(const Tuple& args, const Dict& kw)
{
    FlowProperty::object_python_init(args, kw);

    if (args.size() > 1)
    {
        PyObject* def = args.get(1);
        if (PyUnicode_Check(def))
        {
            _default_value = args.get(1);
            Py_XINCREF(_default_value);
        }
        else
        {
            PYTHON_ERROR(TypeError, "Expected default argument to be string");
        }
    }
}