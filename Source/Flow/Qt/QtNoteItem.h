#ifndef __QT_NOTE_ITEM_H__
#define __QT_NOTE_ITEM_H__

#include <QGraphicsItem>

class QGraphicsProxyWidget;
class QTextEdit;
class QtNoteItem : public QGraphicsItem
{
public:
    enum { Type = UserType + 3 };

    QtNoteItem(QGraphicsWidget* parent = nullptr);
    virtual ~QtNoteItem();

    QRectF boundingRect() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;

    void set_text(const QString& txt) const;
    QString text() const;

    int type() const;

private:
    QGraphicsProxyWidget* _proxy;
    QTextEdit* _text_edit;

};

#endif // __QT_NOTE_ITEM_H__