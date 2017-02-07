#ifndef __FLOW_QT_TEXT_VIEW_NODE_H__
#define __FLOW_QT_TEXT_VIEW_NODE_H__

#include "Qt/QtFlowNode.h"

class QTextEdit;

class QtTextViewNode : public QtFlowNode
{
    Q_OBJECT

public:
    QtTextViewNode(FlowNode* node, QGraphicsObject* parent = nullptr);
    virtual ~QtTextViewNode();

    virtual void node_updated() OVERRIDE;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;

public slots:
    void set_text(const QString& text);

protected:
    virtual void create_pins() OVERRIDE;
    virtual void calculate_size() OVERRIDE;

private:
    QTextEdit* _text_edit;
};

#endif // __FLOW_QT_TEXT_VIEW_NODE_H__
