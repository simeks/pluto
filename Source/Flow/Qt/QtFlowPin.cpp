//#include <Core/Common.h>
//
//#include "FlowPin.h"
//#include "QtFlowLink.h"
//#include "QtFlowNode.h"
//#include "QtFlowPin.h"
//
//#include <QLabel>
//#include <QPainter>
//
//QtFlowPin::QtFlowPin() :
//    _pin(nullptr),
//    _owner(nullptr)
//{
//}
//QtFlowPin::QtFlowPin(FlowPin* pin, QtFlowNode* owner) :
//    _pin(pin),
//    _owner(owner)
//{
//    QLabel* label = new QLabel(">");
//    if (_pin->pin_type() == FlowPin::In)
//        label->setContentsMargins(5, 0, 0, 0);
//    else
//        label->setContentsMargins(0, 0, 5, 0);
//
//    label->setAttribute(Qt::WA_TranslucentBackground);
//    setWidget(label);
//}
//QtFlowPin::~QtFlowPin()
//{
//}
//
//void QtFlowPin::set_highlighted(bool highlight)
//{
//    QLabel* label = (QLabel*)widget();
//    if (highlight)
//    {
//        label->setText("O");
//    }
//    else
//    {
//        label->setText(">");
//    }
//}
//FlowPin* QtFlowPin::pin() const
//{
//    return _pin;
//}
//FlowPin::Type QtFlowPin::pin_type() const
//{
//    if(_pin)
//        return _pin->pin_type();
//    return FlowPin::Unknown;
//}
//int QtFlowPin::pin_id() const
//{
//    if (_pin)
//        return _pin->pin_id();
//    return -1;
//}
//QtFlowNode* QtFlowPin::owner() const
//{
//    return _owner;
//}
//int QtFlowPin::type() const
//{
//    return Type;
//}
