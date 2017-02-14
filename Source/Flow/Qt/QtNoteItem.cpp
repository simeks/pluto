#include <Core/Common.h>

#include "GraphNote.h"
#include "QtNoteItem.h"

#include <QGraphicsProxyWidget>
#include <QPainter>
#include <QTextEdit>


QtNoteItem::QtNoteItem(GraphNote* note, QGraphicsWidget* parent) : QGraphicsObject(parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    _note = note;
    _note->addref();

    setPos(_note->ui_pos().x, _note->ui_pos().y);

    _text_edit = new QTextEdit();
    _text_edit->setWindowFlags(Qt::FramelessWindowHint);
    _text_edit->setAttribute(Qt::WA_TranslucentBackground);
    _text_edit->setStyleSheet("background: transparent; border: none; color: white; font-size:9pt;");
    _text_edit->setFixedSize(190, 50);
    _text_edit->setText(note->text());

    connect(_text_edit, SIGNAL(textChanged()), this, SLOT(text_changed()));

    _proxy = new QGraphicsProxyWidget(this);
    _proxy->setWidget(_text_edit);
    _proxy->setPos(5, 5);
}
QtNoteItem::QtNoteItem(QGraphicsWidget* parent) : QGraphicsObject(parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    _note = object_new<GraphNote>();

    _text_edit = new QTextEdit();
    _text_edit->setWindowFlags(Qt::FramelessWindowHint);
    _text_edit->setAttribute(Qt::WA_TranslucentBackground);
    _text_edit->setStyleSheet("background: transparent; border: none; color: white; font-size:9pt;");
    _text_edit->setFixedSize(190, 50);

    connect(_text_edit, SIGNAL(textChanged()), this, SLOT(text_changed()));

    _proxy = new QGraphicsProxyWidget(this);
    _proxy->setWidget(_text_edit);
    _proxy->setPos(5, 5);
}
QtNoteItem::~QtNoteItem()
{
    if (_note)
        _note->release();
}
QRectF QtNoteItem::boundingRect() const
{
    return QRectF(-10, -10, 210, 70);
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
    painter->drawRect(QRectF(0, 0, 200, 60));

    painter->setBrush(QColor::fromRgba(qRgba(0, 122, 204, 255)));
    painter->setPen(QColor::fromRgba(qRgba(0, 80, 180, 255)));
    painter->drawEllipse(QPoint(2, 2), 7, 7);
}
void QtNoteItem::set_text(const QString& txt) const
{
    _note->set_text(txt.toUtf8().constData());
    _text_edit->setText(txt);
}
QString QtNoteItem::text() const
{
    return _text_edit->toPlainText();
}
GraphNote* QtNoteItem::note() const
{
    return _note;
}
int QtNoteItem::type() const
{
    return Type;
}
void QtNoteItem::text_changed()
{
    _note->set_text(_text_edit->toPlainText().toUtf8().constData());
}
QVariant QtNoteItem::itemChange(GraphicsItemChange change, const QVariant & value)
{
    if (change == ItemPositionHasChanged)
    {
        QPointF pos = value.toPointF();
        _note->set_ui_pos(Vec2i(int(pos.x()), int(pos.y())));
    }
    return QGraphicsObject::itemChange(change, value);
}
