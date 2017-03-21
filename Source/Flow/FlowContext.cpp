#include <Core/Common.h>
#include <Core/Python/PythonFunction.h>

#include "FlowContext.h"
#include "FlowGraph.h"
#include "FlowNode.h"
#include "FlowPin.h"
#include "GraphInputNode.h"
#include "GraphOutputNode.h"

#include <QTemporaryDir>

PYTHON_OBJECT_IMPL(FlowContext, "Context")
{
    cls.def_init<FlowContext, FlowGraph*>();
    cls.def("write_pin", &FlowContext::write_pin, "");
    cls.def("read_pin", &FlowContext::read_pin, "");
    cls.def("is_pin_linked", &FlowContext::is_pin_linked, "");
    cls.def("temp_dir", &FlowContext::temp_dir, "");
    cls.def("temp_node_dir", &FlowContext::temp_node_dir, "");
}

FlowContext::FlowContext(FlowGraph* graph) :
    _temp_dir(new QTemporaryDir()),
    _graph(graph),
    _current_node(nullptr),
    _failed(false)
{
}
FlowContext::~FlowContext()
{
    clean_up();
    delete _temp_dir;
}
bool FlowContext::run(Callback* cb)
{
    std::map<FlowNode*, int> incoming_edges;

    // Only perform initialization if last run didn't fail
    if (!_failed)
    {
        _nodes_to_execute.clear();

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
    }
    else
    {
        // Reload nodes
        for (auto it = _nodes_to_execute.begin(); it != _nodes_to_execute.end(); ++it)
        {
            (*it) = _graph->node((*it)->node_id());
        }
        reset_error();
    }

    std::set<FlowNode*> next;
    while (!_nodes_to_execute.empty())
    {
        _current_node = _nodes_to_execute.back();

        std::cout << "Running " << _current_node->category() << "/" << _current_node->title() << std::endl;

        if (cb)
            cb->node_started(_current_node);

        _current_node->run(this);

        if (failed())
        {
            if (cb)
                cb->node_failed(_current_node);

            return false;
        }
        else if (PyErr_Occurred())
        {
            PyObject *type, *value, *traceback;
            PyErr_Fetch(&type, &value, &traceback);

            if (type && value)
            {
                std::stringstream ss;
                ss << ((PyTypeObject*)type)->tp_name << ": " << PyUnicode_AsUTF8(PyObject_Str(value));
                _error = ss.str();
            }
            else
            {
                _error = "Unknown error";
            }

            // Kinda ugly but it works
            PyErr_Restore(type, value, traceback);
            PyErr_Print();

            if (cb)
                cb->node_failed(_current_node);

            _failed = true;
            return false;
        }
        else
        {
            if (cb)
                cb->node_finished(_current_node);
        }
        _nodes_to_execute.pop_back();

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
    return true;
}
void FlowContext::clean_up()
{
    _state.clear();
    _env_dict.clear();
    _nodes_to_execute.clear();
}
FlowContext* FlowContext::create_child_context(FlowGraph* graph)
{
    FlowContext* context = python::make_object<FlowContext>(graph);
    context->_env_dict = _env_dict;
    return context;
}
FlowNode* FlowContext::current_node()
{
    return _current_node;
}
void FlowContext::write_pin(const char* name, const python::Object& obj)
{
    if (_current_node)
    {
        FlowPin* pin = _current_node->pin(name);
        if (pin && pin->pin_type() == FlowPin::Out)
        {
            _state[pin] = obj;
        }
        // else TODO: Error
    }
}
python::Object FlowContext::read_pin(const char* name)
{
    if (_current_node)
    {
        FlowPin* pin = _current_node->pin(name);
        if (pin)
        {
            if (!pin->links().empty() && pin->pin_type() == FlowPin::In)
            {
                auto it = _state.find(pin->links()[0]);
                if (it != _state.end())
                {
                    return it->second;
                }
            }
        }
        else
        {
            // If no single pin was found, try for pin array

            std::vector<ArrayFlowPin*> pin_array = _current_node->pin_array(name);
            if (pin_array.size() > 1)
            {
                // The way pin array work we always have n + 1 pins when n is the number of linked pins

                Tuple t(pin_array.size() - 1);
                for (int i = 0; i < pin_array.size() - 1; ++i)
                {
                    auto it = _state.find(pin_array[i]->links()[0]);
                    if (it != _state.end())
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
    if (_current_node)
        return _current_node->is_pin_linked(name);
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
    return _inputs;
}
const std::map<std::string, python::Object>& FlowContext::outputs() const
{
    return _outputs;
}

python::Object FlowContext::input(const char* name) const
{
    auto it = _inputs.find(name);
    if (it != _inputs.end())
        return it->second;
    return python::None();
}
python::Object FlowContext::output(const char* name) const
{
    auto it = _outputs.find(name);
    if (it != _outputs.end())
        return it->second;
    return python::None();
}
void FlowContext::set_input(const char* name, const python::Object& value)
{
    auto it = _inputs.find(name);
    it->second = value;
}
void FlowContext::set_output(const char* name, const python::Object& value)
{
    auto it = _outputs.find(name);
    it->second = value;
}
const char* FlowContext::temp_dir() const
{
    return _temp_dir->path().toUtf8();
}
std::string FlowContext::temp_node_dir() const
{
    if (!_current_node)
        PYTHON_ERROR(PyExc_ValueError, "No current node");

    QString node_id = guid::to_string(_current_node->node_id()).c_str();

    QDir dir(_temp_dir->path());
    if (!dir.exists(node_id))
        dir.mkdir(node_id);

    dir = QDir::cleanPath(dir.path() + QDir::separator() + node_id);
    return dir.path().toStdString();
}
bool FlowContext::failed() const
{
    return _failed;
}
const char* FlowContext::error() const
{
    return _error.c_str();
}
void FlowContext::raise_error(const char* error)
{
    _failed = true;
    _error = error;
}
void FlowContext::reset_error()
{
    _failed = false;
    _error = "";
}
void FlowContext::initialize()
{
    if (!_graph)
        return;

    set_attribute("env", _env_dict);

    for (auto& n : _graph->nodes())
    {
        if (n.second->is_a(GraphInputNode::static_class()))
        {
            _inputs[n.second->attribute<const char*>("name")] = python::None();
        }
        else if (n.second->is_a(GraphOutputNode::static_class()))
        {
            _outputs[n.second->attribute<const char*>("name")] = python::None();
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
