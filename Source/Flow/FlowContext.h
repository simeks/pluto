#ifndef __FLOW_CONTEXT_H__
#define __FLOW_CONTEXT_H__

#include "API.h"
#include <Core/Object/Object.h>
#include <Core/Python/Dict.h>

class FlowGraph;
class FlowNode;
class FlowPin;
class GraphInputNode;
class GraphOutputNode;
class QTemporaryDir;

struct FlowGraphState;

class FLOW_API FlowContext : public Object
{
    PLUTO_OBJECT(FlowContext, Object);

public:
    FlowContext(FlowGraphState* state);
    ~FlowContext();

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

    template<typename T>
    void write_pin_(const char* name, const T& obj)
    {
        write_pin(name, python::Object(python::to_python(obj)));
    }
    template<typename T>
    T read_pin_(const char* name)
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
    T read_pin_(const char* name, const T& def)
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

    FlowGraphState* _state;
    python::Dict _env_dict;

    QTemporaryDir* _temp_dir;
};

#endif // __FLOW_CONTEXT_H__
