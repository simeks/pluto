#ifndef __QT_VIS_NODE_H__
#define __QT_VIS_NODE_H__

#include "API.h"

#include <Core/Image/Image.h>
#include <Core/Image/Vec3.h>
#include <Flow/Qt/QtFlowNode.h>

class Image;
class QtVisWindow;
class VIS_API QtVisNode : public QtFlowNode
{
    Q_OBJECT

public:
    QtVisNode(FlowNode* node, QGraphicsObject* parent = nullptr);
    virtual ~QtVisNode();
    
    virtual void node_updated() OVERRIDE;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) OVERRIDE;

public slots:
    void show_image(const Image& image);

protected:
    virtual void create_pins() OVERRIDE;
    virtual void calculate_size() OVERRIDE;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    QtFlowPin* _pin;
    QImage _qimage;
    QImage _thumbnail;
    NumpyArray _data;
    Vec3d _spacing;

    QtVisWindow* _window;
};

#endif // __QT_VIS_NODE_H__
