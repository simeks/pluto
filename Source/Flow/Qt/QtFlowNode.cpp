#include <Core/Common.h>

#include "FlowNode.h"
#include "QtFlowLink.h"
#include "QtFlowNode.h"
#include "QtFlowPin.h"

#include <QLabel>
#include <QGraphicsGridLayout>
#include <QGraphicsProxyWidget>


QtFlowNode::QtFlowNode(FlowNode* node, QGraphicsItem* parent) : QtBaseNode(node, parent)
{
    setup();
}
QtFlowNode::~QtFlowNode()
{
}
void QtFlowNode::setup()
{
    QGraphicsProxyWidget* name_label = new QGraphicsProxyWidget(this);
    name_label->setWidget(new QLabel(_node->title()));
    name_label->widget()->setContentsMargins(5, 2, 5, 2);
    name_label->widget()->setAttribute(Qt::WA_TranslucentBackground);
    _layout->addItem(name_label, 0, 0, Qt::AlignLeft | Qt::AlignTop);

    int in_pins = 1, out_pins = (int)_node->pins().size();
    for (auto& pin : _node->pins())
    {
        QtFlowPin* pin_icon = new QtFlowPin(pin, this);
        pin_icon->setMaximumWidth(15);
        _pins.push_back(pin_icon);

        QGraphicsProxyWidget* label = new QGraphicsProxyWidget(this);

        QString pin_label = QString("%1").arg(QString::fromStdString(pin->name()));
        label->setWidget(new QLabel(pin_label));
        label->widget()->setAttribute(Qt::WA_TranslucentBackground);

        QGraphicsGridLayout* pin_layout = new QGraphicsGridLayout(_layout);
        pin_layout->setContentsMargins(5, 2, 5, 2);
        if (pin->pin_type() == FlowPin::In)
        {
            _layout->addItem(pin_layout, in_pins, 0, Qt::AlignLeft | Qt::AlignTop);

            pin_layout->addItem(pin_icon, 0, 0, Qt::AlignLeft | Qt::AlignTop);
            pin_layout->addItem(label, 0, 1, Qt::AlignLeft | Qt::AlignTop);
            ++in_pins;
        }
        else
        {
            _layout->addItem(pin_layout, out_pins, 0, Qt::AlignRight | Qt::AlignBottom);

            pin_layout->addItem(label, 0, 0, Qt::AlignRight | Qt::AlignTop);
            pin_layout->addItem(pin_icon, 0, 1, Qt::AlignRight | Qt::AlignTop);
            --out_pins;
        }
    }
}

int QtFlowNode::type() const
{
    return Type;
}
