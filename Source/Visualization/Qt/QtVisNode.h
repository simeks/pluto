#ifndef __QT_VIS_NODE_H__
#define __QT_VIS_NODE_H__

#include "API.h"

#include <Core/Image/Image.h>
#include <Flow/Qt/QtFlowNode.h>

class Image;
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

    QtFlowPin* _pin;
    QImage _qimage;
    NumpyArray _data;
};

#endif // __QT_VIS_NODE_H__
