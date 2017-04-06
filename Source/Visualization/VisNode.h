#ifndef __VIS_NODE_H__
#define __VIS_NODE_H__

#include "API.h"

#include <Flow/FlowNode.h>

class QtVisWindow;
class VIS_API VisNode : public FlowNode
{
    PLUTO_OBJECT(VisNode, FlowNode);

public:
    VisNode();
    ~VisNode();

    void run(FlowContext* ctx) OVERRIDE;
    
private:
    QtVisWindow* _current_window;
};


#endif // __VIS_NODE_H__
