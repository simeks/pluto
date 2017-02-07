#ifndef __VIS_NODE_H__
#define __VIS_NODE_H__

#include "API.h"

#include <Flow/UiFlowNode.h>

class QtVisWindow;
class VIS_API VisNode : public UiFlowNode
{
    DECLARE_OBJECT(VisNode, UiFlowNode);

public:
    DECLARE_OBJECT_CONSTRUCTOR(VisNode);
    ~VisNode();

    void object_init();
    
    void run(FlowContext* ctx) OVERRIDE;
    
private:
    QtVisWindow* _current_window;
};


#endif // __VIS_NODE_H__
