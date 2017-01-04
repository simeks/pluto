#include <Core/Common.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowNode.h"
#include "FlowPin.h"

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2(FlowContext, write_pin, const char*, PyObject*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowContext, read_pin, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(FlowContext, run, FlowGraph*);

OBJECT_INIT_TYPE_FN(FlowContext)
{
    OBJECT_PYTHON_ADD_METHOD(FlowContext, write_pin, "");
    OBJECT_PYTHON_ADD_METHOD(FlowContext, read_pin, "");
    OBJECT_PYTHON_ADD_METHOD(FlowContext, run, "");
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
void FlowContext::run(FlowGraph* graph, Callback* cb)
{
    _nodes_to_execute.clear();

    std::map<FlowNode*, int> incoming_edges;

    // Find all nodes without inputs
    for (auto n : graph->nodes())
    {
        int edges = 0;
        for (auto p : n.second->pins())
        {
            if (p->pin_type() == FlowPin::In && !p->links().empty())
                ++edges;
        }
        incoming_edges[n.second] = edges;
        if (edges == 0)
            _nodes_to_execute.push_back(n.second);
    }

    std::set<FlowNode*> next;
    while (!_nodes_to_execute.empty())
    {
        _current_node = _nodes_to_execute.back();
        _nodes_to_execute.pop_back();
        
        std::cout << "Running " << _current_node->category() << "/" << _current_node->title() << std::endl;

        if (cb)
            cb->node_started(_current_node);

        _current_node->run(this);

        if (cb)
            cb->node_finished(_current_node);

        next.clear();
        find_dependents(_current_node, next);
        for (auto n : next)
        {
            for (auto p : n->pins())
            {
                if (p->pin_type() == FlowPin::In && !p->links().empty() && p->links()[0]->owner() == _current_node)
                    --incoming_edges[n];
            }
            if (incoming_edges[n] == 0)
                _nodes_to_execute.push_back(n);
        }
        _current_node = nullptr;
    }
}
void FlowContext::clean_up()
{
    for (auto o : _state)
    {
        Py_XDECREF(o.second);
    }
    _state.clear();
    _env_dict.clear();
    _nodes_to_execute.clear();
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
        if (pin && !pin->links().empty() && pin->pin_type() == FlowPin::In)
        {
            auto it = _state.find(pin->links()[0]);
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
void FlowContext::find_dependents(FlowNode* node, std::set<FlowNode*>& dependents)
{
    for (auto p : node->pins())
    {
        if (p->pin_type() == FlowPin::Out)
        {
            for (auto p2 : p->links())
            {
                dependents.emplace(p2->owner());
            }
        }
    }
}
