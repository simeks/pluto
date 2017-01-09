#include <Core/Common.h>

#include "QtNoteItem.h"

#include <QGraphicsProxyWidget>
#include <QPainter>
#include <QTextEdit>

QtNoteItem::QtNoteItem(QGraphicsWidget* parent) : QGraphicsItem(parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);

    _text_edit = new QTextEdit();
    _text_edit->setWindowFlags(Qt::FramelessWindowHint);
    _text_edit->setAttribute(Qt::WA_TranslucentBackground);
    _text_edit->setStyleSheet("background: transparent; border: none; color: white; font-size:9pt;");
    _text_edit->setFixedSize(190, 90);

    _proxy = new QGraphicsProxyWidget(this);
    _proxy->setWidget(_text_edit);
    _proxy->setPos(5, 5);
}
QtNoteItem::~QtNoteItem()
{
}
QRectF QtNoteItem::boundingRect() const
{
    return QRectF(-10, -10, 210, 110);
}
void QtNoteItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setClipRect(boundingRect());
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QColor::fromRgba(qRgba(100, 100, 100, 150)));

    QPen pen;
    if (isSelected())
        pen.setColor(QColor::fromRgba(qRgba(255, 255, 255, 255)));
    else
        pen.setColor(QColor::fromRgba(qRgba(180, 180, 180, 200)));
    
    painter->setPen(pen);
    painter->drawRect(QRectF(0, 0, 200, 100));

    painter->setBrush(QColor::fromRgba(qRgba(0, 122, 204, 255)));
    painter->setPen(QColor::fromRgba(qRgba(0, 80, 180, 255)));
    painter->drawEllipse(QPoint(2, 2), 7, 7);
}
void QtNoteItem::set_text(const QString& txt) const
{
    _text_edit->setText(txt);
}
QString QtNoteItem::text() const
{
    return _text_edit->toPlainText();
}
int QtNoteItem::type() const
{
    return Type;
}
