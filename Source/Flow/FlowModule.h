#ifndef __FLOW_MODULE_H__
#define __FLOW_MODULE_H__

#include "API.h"

#include <Core/Modules/ModuleInterface.h>

class FlowContext;
class FlowNode;
class FlowPythonModule;
class FlowWindow;
class GraphFileLoader;
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

    /// @brief Auto-reload graphs in the given path
    /// First all graphs in the specified path will be loaded and added as nodes in the UI.
    /// If any of the graphs changes while the application is running it will be automatically reloaded.
    void add_graph_path(const char* path);

    QtFlowUI* ui() const;

    static FlowModule& instance();

private:
    static FlowModule* s_instance;
    
    QtFlowUI* _ui;

    std::vector<FlowNode*> _node_templates;
    std::vector<FlowWindow*> _flow_windows;

    std::vector<GraphFileLoader*> _loaders;
};



#endif // __FLOW_MODULE_H__
