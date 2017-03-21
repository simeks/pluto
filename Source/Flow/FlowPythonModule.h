#ifndef __FLOW_PYTHON_MODULE_H__
#define __FLOW_PYTHON_MODULE_H__

class FlowContext;
class FlowGraph;
class FlowNode;
class FlowWindow;
class GraphFileLoader;

namespace flow
{
    /// Window handling
    
    /// @brief Loads a graph from the specified file into a new editor window.
    std::unique_ptr<FlowWindow> open(const char* file);

    /// @brief Opens a new window.
    std::unique_ptr<FlowWindow> window();

    /// Graph management

    /// @brief Loads graph from a file
    FlowGraph* load(const char* file);

    /// @brief Saves the given graph to a file
    void save(const char* file, FlowGraph* graph);

    /// @brief Runs a graph
    python::Dict run(const python::Tuple& args, const python::Dict& kw);

    /// Graph node management

    FlowNode* create_node(const char* node_class);

    /// @brief Returns a tuple with all node templates
    python::Tuple node_templates();

    void install_python_module();

}

#endif // __FLOW_PYTHON_MODULE_H__
