#include <Core/Common.h>
#include <Core/Python/Tuple.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowGraphState.h"
#include "FlowNode.h"
#include "FlowPin.h"
#include "GraphInputNode.h"
#include "GraphOutputNode.h"

#include <QTemporaryDir>

PLUTO_OBJECT_IMPL(FlowContext, "Context")
{
    cls.def("write_pin", &FlowContext::write_pin, "");
    cls.def("read_pin", &FlowContext::read_pin, "");
    cls.def("is_pin_linked", &FlowContext::is_pin_linked, "");
    cls.def("temp_dir", &FlowContext::temp_dir, "");
    cls.def("temp_node_dir", &FlowContext::temp_node_dir, "");
}

FlowContext::FlowContext(FlowGraphState* state) :
    _state(state),
    _temp_dir(new QTemporaryDir())
{
}
FlowContext::~FlowContext()
{
    delete _temp_dir;
}
FlowContext* FlowContext::create_child_context(FlowGraph*)
{/*
    FlowContext* context = make_object<FlowContext>(graph);
    context->_env_dict = _env_dict;
    return context;*/
    return nullptr;
}
FlowNode* FlowContext::current_node()
{
    return _state->current_node;
}
void FlowContext::write_pin(const char* name, const python::Object& obj)
{
    if (_state->current_node)
    {
        FlowPin* pin = _state->current_node->pin(name);
        if (pin && pin->pin_type() == FlowPin::Out)
        {
            _state->pins[pin] = obj;
        }
        // else TODO: Error
    }
}
python::Object FlowContext::read_pin(const char* name)
{
    if (_state->current_node)
    {
        FlowPin* pin = _state->current_node->pin(name);
        if (pin)
        {
            if (!pin->links().empty() && pin->pin_type() == FlowPin::In)
            {
                auto it = _state->pins.find(pin->links()[0]);
                if (it != _state->pins.end())
                {
                    return it->second;
                }
            }
        }
        else
        {
            // If no single pin was found, try for pin array

            std::vector<ArrayFlowPin*> pin_array = _state->current_node->pin_array(name);
            if (pin_array.size() > 1)
            {
                // The way pin array work we always have n + 1 pins when n is the number of linked pins

                python::Tuple t(pin_array.size() - 1);
                for (int i = 0; i < pin_array.size() - 1; ++i)
                {
                    auto it = _state->pins.find(pin_array[i]->links()[0]);
                    if (it != _state->pins.end())
                    {
                        t.set(i, it->second);
                    }
                }
                return t;
            }
        }
        // else TODO: Error
    }
    Py_RETURN_NONE;
}
bool FlowContext::is_pin_linked(const char* name)
{
    if (_state->current_node)
        return _state->current_node->is_pin_linked(name);
    return false;
}
bool FlowContext::has_env_var(const char* key) const
{
    return _env_dict.has_key(key);
}
const char* FlowContext::env_get(const char* key) const
{
    python::Object obj = _env_dict.get(key);
    if (PyUnicode_Check(obj.ptr()))
    {
        return PyUnicode_AsUTF8(obj.ptr());
    }
    return nullptr;
}
void FlowContext::env_set(const char* key, const char* value)
{
    _env_dict.set(key, PyUnicode_FromString(value));
}

const std::map<std::string, python::Object>& FlowContext::inputs() const
{
    return _state->inputs;
}
const std::map<std::string, python::Object>& FlowContext::outputs() const
{
    return _state->outputs;
}

python::Object FlowContext::input(const char* name) const
{
    auto it = _state->inputs.find(name);
    if (it != _state->inputs.end())
        return it->second;
    return python::None();
}
python::Object FlowContext::output(const char* name) const
{
    auto it = _state->outputs.find(name);
    if (it != _state->outputs.end())
        return it->second;
    return python::None();
}
void FlowContext::set_input(const char* name, const python::Object& value)
{
    auto it = _state->inputs.find(name);
    it->second = value;
}
void FlowContext::set_output(const char* name, const python::Object& value)
{
    auto it = _state->outputs.find(name);
    it->second = value;
}
const char* FlowContext::temp_dir() const
{
    return _temp_dir->path().toUtf8();
}
std::string FlowContext::temp_node_dir() const
{
    if (!_state->current_node)
        PYTHON_ERROR(PyExc_ValueError, "No current node");

    QString node_id = guid::to_string(_state->current_node->node_id()).c_str();

    QDir dir(_temp_dir->path());
    if (!dir.exists(node_id))
        dir.mkdir(node_id);

    dir = QDir::cleanPath(dir.path() + QDir::separator() + node_id);
    return dir.path().toStdString();
}
void FlowContext::initialize()
{
    if (!_state->graph)
        return;

    set_attribute("env", _env_dict);
}

