#include <Core/Common.h>

#include "FlowNode.h"
#include "FlowPin.h"
#include "Qt/QtFlowPin.h"
#include "Qt/Style.h"
#include "QtTextViewNode.h"

#include <QFontMetrics>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QPainter>
#include <QTextEdit>

QtTextViewNode::QtTextViewNode(FlowNode* node, QGraphicsObject* parent) :
    QtFlowNode(node, parent)
{
    _text_edit = new QTextEdit();
    _text_edit->setEnabled(false);
    _text_edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _text_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _text_edit->setLineWrapMode(QTextEdit::NoWrap);

    _text_edit->setWindowFlags(Qt::FramelessWindowHint);
    _text_edit->setAttribute(Qt::WA_TranslucentBackground);
    _text_edit->setAttribute(Qt::WA_TransparentForMouseEvents);
    _text_edit->setStyleSheet("background: transparent; border: none; color: white; font-size:9pt;");

    _text_edit->resize(75, 40);
    _text_edit->move(5, 30);

    QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget(this);
    proxy->setEnabled(false);
    proxy->setAcceptedMouseButtons(0);
    proxy->setWidget(_text_edit);

    proxy->setFlag(QGraphicsItem::ItemIsSelectable, false);
    proxy->setFlag(QGraphicsItem::ItemIsMovable, false);
    proxy->setFlag(QGraphicsItem::ItemIsFocusable, false);
}
QtTextViewNode::~QtTextViewNode()
{
    delete _text_edit;
    _text_edit = nullptr;
}
void QtTextViewNode::node_updated()
{
}
void QtTextViewNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setClipRect(boundingRect());
    painter->setRenderHint(QPainter::Antialiasing);

    const FlowUIStyle& style = FlowUIStyle::default_style();

    painter->setBrush(QBrush(style.node_background_0));

    if (isSelected())
        painter->setPen(QPen(style.node_border_selected_0, 1));
    else
        painter->setPen(QPen(style.node_border_0, 1));

    painter->drawRect(_rect);

    QFont fnt = style.node_font;
    fnt.setBold(true);
    QFontMetrics metrics(fnt);

    QPoint title_pos((_rect.width() - metrics.width(_title)) / 2, 20);

    painter->setFont(fnt);
    painter->setPen(style.node_title_color);

    painter->drawText(title_pos, _title);

    QtFlowPin* pin = _pins[0];
    painter->setBrush(pin->color());
    painter->setPen(pin->outline_color());
    painter->drawEllipse(pin->local_pos(), style.pin_radius, style.pin_radius);
}
void QtTextViewNode::set_text(const QString& text)
{
    _text_edit->setHtml(text);

    QSizeF size = _text_edit->document()->size();
    _text_edit->resize(ceil(size.width()), ceil(size.height()));

    _rect = QRect(0, 0, _text_edit->width() + 10, _text_edit->height() + 40);

    update();
}
void QtTextViewNode::create_pins()
{
    assert(_node->pins().size() == 1);

    const FlowUIStyle& style = FlowUIStyle::default_style();

    QFontMetrics metrics(style.node_font);

    FlowPin* pin = _node->pins()[0];
    QPoint pin_pos = QPoint(0, 10 + metrics.height()/2);

    _pins.push_back(new QtFlowPin(this, pin, pin_pos));
}
void QtTextViewNode::calculate_size()
{
    assert(_node->pins().size() == 1);
    _rect = QRect(0, 0, _text_edit->width()+10, _text_edit->height()+40);
}


