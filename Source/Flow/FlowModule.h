#ifndef __FLOW_MODULE_H__
#define __FLOW_MODULE_H__

#include <Core/Modules/ModuleInterface.h>
#include <Core/Python/PythonModule.h>

class FlowContext;
class FlowNode;
class FlowPythonModule;
class FlowWindow;
class QtFlowUI;
struct FlowNodeDef;
class FlowModule : public ModuleInterface
{
public:
    FlowModule();
    ~FlowModule();

    void install();
    void uninstall();

    void install_node_template(FlowNode* node);
    void install_node_template(const FlowNodeDef& def);

    FlowNode* node_template(const char* node_class) const;

    const std::vector<FlowNode*>& node_templates() const;

    static FlowModule& instance();

private:
    static FlowModule* s_instance;
    
    FlowPythonModule* _py_module;
    QtFlowUI* _ui;

    std::vector<FlowNode*> _node_templates;
    std::vector<FlowWindow*> _flow_windows;
};



#endif // __FLOW_MODULE_H__
