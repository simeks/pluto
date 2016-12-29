#ifndef __FLOW_CONTEXT_H__
#define __FLOW_CONTEXT_H__

#include "API.h"
#include <Core/Object/Object.h>

class FlowNode;
class FlowPin;
class FLOW_API FlowContext : public Object
{
    DECLARE_OBJECT(FlowContext, Object);

public:
    DECLARE_OBJECT_CONSTRUCTOR(FlowContext);
    ~FlowContext();

    void object_init();
    void object_python_init(const Tuple& args, const Dict& kw);

    FlowNode* current_node();

    void write_pin(const char* name, PyObject* obj);
    PyObject* read_pin(const char* name);
    
    bool has_env_var(const char* key) const;
    const char* env_get(const char* key) const;
    void env_set(const char* key, const char* value);

private:
    std::map<FlowPin*, PyObject*> _state;
    Dict _env_dict;

    FlowNode* _current_node;
};

#endif // __FLOW_CONTEXT_H__
