#ifndef __FLOW_CONTEXT_H__
#define __FLOW_CONTEXT_H__

#include "API.h"
#include <Core/Object/Object.h>

class FlowGraph;
class FlowNode;
class FlowPin;
class GraphInputNode;
class GraphOutputNode;
class QTemporaryDir;
class FLOW_API FlowContext : public Object
{
    DECLARE_OBJECT(FlowContext, Object);

public:
    class Callback
    {
    public:
        virtual void node_started(FlowNode*) {}
        virtual void node_finished(FlowNode*) {}
        virtual void node_failed(FlowNode*) {}
    };
    
    DECLARE_OBJECT_CONSTRUCTOR(FlowContext);
    ~FlowContext();

    void object_init(FlowGraph* graph);
    void object_python_init(const Tuple& args, const Dict& kw);

    /// Returns true if the run was successful, false if not.
    /// Run will continue from previous state if last run failed.
    bool run(Callback* cb = nullptr);
    void clean_up();

    FlowContext* create_child_context(FlowGraph* graph);

    FlowNode* current_node();

    void write_pin(const char* name, const python::Object& obj);
    python::Object read_pin(const char* name);
    
    bool is_pin_linked(const char* name);

    bool has_env_var(const char* key) const;
    const char* env_get(const char* key) const;
    void env_set(const char* key, const char* value);

    const std::map<std::string, python::Object>& inputs() const;
    const std::map<std::string, python::Object>& outputs() const;

    /// Returns a borrowed reference
    python::Object input(const char* name) const;
    /// Returns a borrowed reference
    python::Object output(const char* name) const;

    void set_input(const char* name, const python::Object& value);
    void set_output(const char* name, const python::Object& value);

    const char* temp_dir() const;

    /// Returns the path to the temporary directory for the current node.
    ///     Creates a new directory if there is none
    std::string temp_node_dir() const;

    bool failed() const;
    const char* error() const;

    void raise_error(const char* error);
    void reset_error();

    template<typename T>
    void write_pin(const char* name, const T& obj)
    {
        write_pin(name, python::Object(python::to_python(obj)));
    }
    template<typename T>
    T read_pin(const char* name)
    {
        T ret = python::from_python<T>(read_pin(name));
        if (PyErr_Occurred())
        {
            // TODO:
            PyErr_Print();
            return ret;
        }

        return ret;
    }
    template<typename T>
    T read_pin(const char* name, const T& def)
    {
        PyObject* obj = read_pin(name);
        if (!obj)
            return def;

        T ret = python::from_python<T>(obj);
        if (PyErr_Occurred())
        {
            // TODO:
            PyErr_Print();
            return def;
        }

        return ret;
    }

private:
    void initialize();
    void find_dependents(FlowNode* node, std::set<FlowNode*>& dependents);

    FlowGraph* _graph;
    std::map<FlowPin*, python::Object> _state;
    Dict _env_dict;

    FlowNode* _current_node;
    std::vector<FlowNode*> _nodes_to_execute;

    std::map<std::string, python::Object> _inputs;
    std::map<std::string, python::Object> _outputs;

    QTemporaryDir* _temp_dir;

    bool _failed;
    std::string _error;
};

#endif // __FLOW_CONTEXT_H__
