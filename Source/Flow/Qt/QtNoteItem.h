#ifndef __QT_NOTE_ITEM_H__
#define __QT_NOTE_ITEM_H__

#include <Core/Common.h>
#include <QGraphicsItem>

class GraphNote;
class QGraphicsProxyWidget;
class QTextEdit;
class QtNoteItem : public QGraphicsObject
{
    Q_OBJECT

public:
    enum { Type = UserType + 3 };

    QtNoteItem(GraphNote* note, QGraphicsWidget* parent = nullptr);
    QtNoteItem(QGraphicsWidget* parent = nullptr);
    virtual ~QtNoteItem();

    QRectF boundingRect() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;

    void set_text(const QString& txt) const;
    QString text() const;

    GraphNote* note() const;

    int type() const;

private slots:
    void text_changed();

protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value) OVERRIDE;

private:
    GraphNote* _note;

    QGraphicsProxyWidget* _proxy;
    QTextEdit* _text_edit;

};

#endif // __QT_NOTE_ITEM_H__