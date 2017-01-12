#ifndef __FLOW_MODULE_H__
#define __FLOW_MODULE_H__

#include "API.h"

#include <Core/Modules/ModuleInterface.h>
#include <Core/Python/PythonModule.h>

class FlowContext;
class FlowNode;
class FlowPythonModule;
class FlowWindow;
class QtFlowUI;
struct FlowNodeDef;
class FLOW_API FlowModule : public ModuleInterface
{
public:
    FlowModule();
    ~FlowModule();

    void install() OVERRIDE;
    void uninstall() OVERRIDE;

    void init() OVERRIDE;

    void install_node_template(FlowNode* node);
    void install_node_template(const FlowNodeDef& def);

    FlowNode* node_template(const char* node_class) const;

    const std::vector<FlowNode*>& node_templates() const;

    static FlowModule& instance();

private:
    static FlowModule* s_instance;
    
    QtFlowUI* _ui;

    std::vector<FlowNode*> _node_templates;
    std::vector<FlowWindow*> _flow_windows;
};



#endif // __FLOW_MODULE_H__
