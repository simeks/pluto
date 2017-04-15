#ifndef __FLOW_QT_PRINT_NODE_H__
#define __FLOW_QT_PRINT_NODE_H__

#include "QtSinglePinNode.h"

class QtPrintNode : public QtSinglePinNode
{
    Q_OBJECT

public:
    QtPrintNode(FlowNode* node, QGraphicsObject* parent = nullptr);
    virtual ~QtPrintNode();

    virtual void node_finished() OVERRIDE;
    virtual void reset_run_status() OVERRIDE;

public slots:
    void print_text(const QString& text);
};

#endif // __FLOW_QT_PRINT_NODE_H__
