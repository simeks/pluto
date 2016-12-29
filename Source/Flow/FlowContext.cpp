#include <Core/Common.h>

#include "FlowContext.h"
#include "FlowNode.h"
#include "FlowPin.h"

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2(FlowContext, write_pin, const char*, PyObject*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowContext, read_pin, const char*);

OBJECT_INIT_TYPE_FN(FlowContext)
{
    OBJECT_PYTHON_ADD_METHOD(FlowContext, write_pin, "");
    OBJECT_PYTHON_ADD_METHOD(FlowContext, read_pin, "");
}

IMPLEMENT_OBJECT(FlowContext, "FlowContext", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(FlowContext, Object);

FlowContext::~FlowContext()
{
}
void FlowContext::object_init()
{
    _current_node = nullptr;
    set_attribute("env", _env_dict);
}
void FlowContext::object_python_init(const Tuple& , const Dict& )
{
    _current_node = nullptr;
    set_attribute("env", _env_dict);
}
FlowNode* FlowContext::current_node()
{
    return _current_node;
}
void FlowContext::write_pin(const char* name, PyObject* obj)
{
    if (_current_node)
    {
        FlowPin* pin = _current_node->pin(name);
        if (pin && pin->pin_type() == FlowPin::Out)
        {
            Py_XINCREF(obj);
            _state[pin] = obj;
        }
        // else TODO: Error
    }
}
PyObject* FlowContext::read_pin(const char* name)
{
    if (_current_node)
    {
        FlowPin* pin = _current_node->pin(name);
        if (pin && pin->pin_type() == FlowPin::In)
        {
            auto it = _state.find(pin);
            if (it != _state.end())
                return it->second;
        }
        // else TODO: Error
    }
    return nullptr;
}
bool FlowContext::has_env_var(const char* key) const
{
    return _env_dict.has_key(key);
}
const char* FlowContext::env_get(const char* key) const
{
    PyObject* obj = _env_dict.get(key);
    if (obj && PyUnicode_Check(obj))
    {
        return PyUnicode_AsUTF8(obj);
    }
    return nullptr;
}
void FlowContext::env_set(const char* key, const char* value)
{
    _env_dict.set(key, PyUnicode_FromString(value));
}
