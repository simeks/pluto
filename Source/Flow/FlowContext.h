#ifndef __FLOW_CONTEXT_H__
#define __FLOW_CONTEXT_H__

#include "API.h"
#include <Core/Object/Object.h>

class FlowGraph;
class FlowNode;
class FlowPin;
class GraphInputNode;
class GraphOutputNode;
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

    void run(Callback* cb = nullptr);
    void clean_up();

    FlowContext* create_child_context(FlowGraph* graph);

    FlowNode* current_node();

    void write_pin(const char* name, PyObject* obj);
    PyObject* read_pin(const char* name);
    
    bool has_env_var(const char* key) const;
    const char* env_get(const char* key) const;
    void env_set(const char* key, const char* value);

    const std::map<std::string, PyObject*>& inputs() const;
    const std::map<std::string, PyObject*>& outputs() const;

    PyObject* input(const char* name) const;
    PyObject* output(const char* name) const;

    void set_input(const char* name, PyObject* value);
    void set_output(const char* name, PyObject* value);

private:
    void initialize();
    void find_dependents(FlowNode* node, std::set<FlowNode*>& dependents);

    FlowGraph* _graph;
    std::map<FlowPin*, PyObject*> _state;
    Dict _env_dict;

    FlowNode* _current_node;
    std::vector<FlowNode*> _nodes_to_execute;

    std::map<std::string, PyObject*> _inputs;
    std::map<std::string, PyObject*> _outputs;
};

#endif // __FLOW_CONTEXT_H__
