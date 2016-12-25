#ifndef __FLOW_PYTHON_MODULE_H__
#define __FLOW_PYTHON_MODULE_H__

#include <Core/Python/PythonModule.h>

class FlowContext;
class FlowNode;
class FlowWindow;
class QtFlowUI;
class FlowPythonModule : public PythonModule
{
public:
    FlowPythonModule(QtFlowUI* ui);
    ~FlowPythonModule();

    void init_module() OVERRIDE;

    FlowWindow* open(const char* file);
    FlowWindow* window();

    FlowGraph* load(const char* file);
    void save(const char* file, FlowGraph* graph);

    void install_node_template(FlowNode* node);
    FlowNode* node_template(const char* node_class) const;
    FlowNode* create_node(const char* node_class) const;

    PyObject* node_templates() const;


private:
    QtFlowUI* _ui;

};


#endif // __FLOW_PYTHON_MODULE_H__
