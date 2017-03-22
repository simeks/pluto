#ifndef __QT_NOTE_ITEM_H__
#define __QT_NOTE_ITEM_H__

#include <Core/Common.h>
#include <QGraphicsItem>

class FlowGraph;
class QGraphicsProxyWidget;
class QTextEdit;
class QtNoteItem : public QGraphicsObject
{
    Q_OBJECT

public:
    enum { Type = UserType + 3 };

    QtNoteItem(FlowGraph* graph, const Guid& note_id, QGraphicsWidget* parent = nullptr);
    virtual ~QtNoteItem();

    QRectF boundingRect() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;

    void set_text(const QString& txt) const;
    QString text() const;

    Guid note_id() const;

    int type() const;

private slots:
    void text_changed();

protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value) OVERRIDE;

private:
    FlowGraph* _graph;
    Guid _note_id;

    QGraphicsProxyWidget* _proxy;
    QTextEdit* _text_edit;

};

#endif // __QT_NOTE_ITEM_H__