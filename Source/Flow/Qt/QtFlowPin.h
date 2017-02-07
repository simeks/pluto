#ifndef __QT_FLOW_PIN_H__
#define __QT_FLOW_PIN_H__

#include "API.h"

#include <QColor>
#include <QPointF>

class FlowPin;
class QtFlowNode;
class FLOW_API QtFlowPin
{
public:
    QtFlowPin(QtFlowNode* owner, FlowPin* pin, const QPointF& local_pos);
    ~QtFlowPin();

    void set_highlight(bool h);
    void set_local_pos(const QPointF& pos);

    QPointF local_pos() const;
    QPointF pos() const;

    bool is_linked() const;

    QtFlowNode* owner() const;
    const char* name() const;
    int id() const;

    FlowPin* pin() const;
    int pin_type() const;

    QColor color() const;
    QColor outline_color() const;

private:
    QtFlowNode* _owner;
    FlowPin* _pin;

    QPointF _local_pos; // Position relative to node

    bool _highlighted;
};

#endif // __QT_FLOW_PIN_H__
