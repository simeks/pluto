#ifndef __FLOW_QT_PRINT_NODE_H__
#define __FLOW_QT_PRINT_NODE_H__

#include "QtSinglePinNode.h"

class QtPrintNode : public QtSinglePinNode
{
public:
    QtPrintNode(FlowNode* node, QGraphicsWidget* parent = nullptr);
    virtual ~QtPrintNode();

    virtual void node_updated() OVERRIDE;
    virtual void reset_run_status() OVERRIDE;
};

#endif // __FLOW_QT_PRINT_NODE_H__
