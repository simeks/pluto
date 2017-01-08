#include <Core/Common.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowNode.h"
#include "FlowPin.h"
#include "GraphInputNode.h"
#include "GraphOutputNode.h"

#include <QTemporaryDir>

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2(FlowContext, write_pin, const char*, PyObject*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(FlowContext, read_pin, const char*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0(FlowContext, run);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(FlowContext, temp_dir);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(FlowContext, temp_node_dir);

OBJECT_INIT_TYPE_FN(FlowContext)
{
    OBJECT_PYTHON_ADD_METHOD(FlowContext, write_pin, "");
    OBJECT_PYTHON_ADD_METHOD(FlowContext, read_pin, "");
    OBJECT_PYTHON_ADD_METHOD(FlowContext, run, "");
    OBJECT_PYTHON_ADD_METHOD(FlowContext, temp_dir, "");
    OBJECT_PYTHON_ADD_METHOD(FlowContext, temp_node_dir, "");
}

IMPLEMENT_OBJECT(FlowContext, "FlowContext", FLOW_API);
IMPLEMENT_OBJECT_CONSTRUCTOR(FlowContext, Object);

FlowContext::~FlowContext()
{
    clean_up();
    delete _temp_dir;
}
void FlowContext::object_init(FlowGraph* graph)
{
    _temp_dir = new QTemporaryDir();
    _graph = graph;
    _current_node = nullptr;
    set_attribute("env", _env_dict);

    initialize();
}
void FlowContext::object_python_init(const Tuple& t, const Dict& )
{
    if (t.size() != 1)
        PYTHON_ERROR(ValueError, "expected 1 argument");

    PyObject* o = t.get(0);
    if (!o || !python_object::object(o))
        PYTHON_ERROR(ValueError, "expected a FlowGraph as argument");

    Object* obj = python_object::object(o);
    if (!obj->is_a(FlowGraph::static_class()))
        PYTHON_ERROR(ValueError, "expected a FlowGraph as argument");

    _graph = object_cast<FlowGraph>(obj);

    _current_node = nullptr;
    set_attribute("env", _env_dict);

    initialize();
}
void FlowContext::run(Callback* cb)
{
    _nodes_to_execute.clear();

    std::map<FlowNode*, int> incoming_edges;

    // Find all nodes without inputs
    for (auto& n : _graph->nodes())
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
    for (auto& o : _state)
    {
        Py_XDECREF(o.second);
    }
    for (auto& i : _inputs)
    {
        Py_XDECREF(i.second);
        i.second = nullptr;
    }
    for (auto& i : _outputs)
    {
        Py_XDECREF(i.second);
        i.second = nullptr;
    }

    _state.clear();
    _env_dict.clear();
    _nodes_to_execute.clear();
}
FlowContext* FlowContext::create_child_context(FlowGraph* graph)
{
    FlowContext* context = object_new<FlowContext>(graph);
    context->_env_dict = _env_dict;
    return context;
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
    Py_RETURN_NONE;
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

const std::map<std::string, PyObject*>& FlowContext::inputs() const
{
    return _inputs;
}
const std::map<std::string, PyObject*>& FlowContext::outputs() const
{
    return _outputs;
}

PyObject* FlowContext::input(const char* name) const
{
    auto it = _inputs.find(name);
    if (it != _inputs.end())
        return it->second;
    return nullptr;
}
PyObject* FlowContext::output(const char* name) const
{
    auto it = _outputs.find(name);
    if (it != _outputs.end())
        return it->second;
    return nullptr;
}
void FlowContext::set_input(const char* name, PyObject* value)
{
    auto it = _inputs.find(name);
    Py_XDECREF(it->second);
    Py_XINCREF(value);
    it->second = value;
}
void FlowContext::set_output(const char* name, PyObject* value)
{
    auto it = _outputs.find(name);
    Py_XDECREF(it->second);
    Py_XINCREF(value);
    it->second = value;
}
const char* FlowContext::temp_dir() const
{
    return _temp_dir->path().toUtf8();
}
std::string FlowContext::temp_node_dir() const
{
    if (!_current_node)
        PYTHON_ERROR_R(ValueError, nullptr, "No current node");

    QString node_id = guid::to_string(_current_node->node_id()).c_str();

    QDir dir(_temp_dir->path());
    if (!dir.exists(node_id))
        dir.mkdir(node_id);

    dir = QDir::cleanPath(dir.path() + QDir::separator() + node_id);
    return dir.path().toStdString();
}

void FlowContext::initialize()
{
    if (!_graph)
        return;

    for (auto& n : _graph->nodes())
    {
        if (n.second->is_a(GraphInputNode::static_class()))
        {
            _inputs[n.second->attribute<const char*>("name")] = nullptr;
        }
        else if (n.second->is_a(GraphOutputNode::static_class()))
        {
            _outputs[n.second->attribute<const char*>("name")] = nullptr;
        }
    }

}
void FlowContext::find_dependents(FlowNode* node, std::set<FlowNode*>& dependents)
{
    for (auto p : node->pins())
    {
        if (p->pin_type() == FlowPin::Out)
        {
            for (auto& p2 : p->links())
            {
                dependents.emplace(p2->owner());
            }
        }
    }
}
