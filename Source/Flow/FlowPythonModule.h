#ifndef __FLOW_PYTHON_MODULE_H__
#define __FLOW_PYTHON_MODULE_H__

#include <Core/Python/PythonModule.h>

class FlowContext;
class FlowNode;
class FlowWindow;
class QtFlowUI;
class FlowPythonModule : public PythonModuleHelper<FlowPythonModule>
{
public:
    FlowPythonModule(QtFlowUI* ui);
    ~FlowPythonModule();

    void post_init() OVERRIDE;

    FlowWindow* open(const char* file);
    FlowWindow* window();

    FlowGraph* load(const char* file);
    void save(const char* file, FlowGraph* graph);

    void install_node_template(FlowNode* node);
    FlowNode* node_template(const char* node_class) const;
    FlowNode* create_node(const char* node_class) const;

    void install_graph_node(const char* name, FlowGraph* graph);
    void install_graph_node_from_file(const char* file);

    Tuple node_templates() const;

    Dict run(const Tuple& args, const Dict& kw);

    static const char* name();

private:
    QtFlowUI* _ui;

};


#endif // __FLOW_PYTHON_MODULE_H__
