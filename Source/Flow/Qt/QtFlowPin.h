#ifndef __QT_FLOW_PIN_H__
#define __QT_FLOW_PIN_H__

#include <Flow/FlowPin.h>
#include <QGraphicsProxyWidget>

class QtFlowLink;
class QtBaseNode;
class QtFlowPin : public QGraphicsProxyWidget
{
    Q_OBJECT

public:
    enum { Type = UserType + 3 };

    QtFlowPin();
    QtFlowPin(FlowPin* pin, QtBaseNode* owner);
    ~QtFlowPin();

    void set_highlighted(bool highlight);

    FlowPin* pin() const;
    FlowPin::Type pin_type() const;
    int pin_id() const;

    QtBaseNode* owner() const;

    int type() const;

private:
    FlowPin* _pin;

    QtBaseNode* _owner;
};

#endif // __QT_FLOW_PIN_H__
