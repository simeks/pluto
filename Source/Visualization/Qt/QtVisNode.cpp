#include <Core/Common.h>
#include <Core/Image/Image.h>

#include <Flow/FlowNode.h>
#include <Flow/FlowPin.h>
#include <Flow/Qt/QtFlowPin.h>
#include <Flow/Qt/Style.h>

#include "QtVisNode.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>

QtVisNode::QtVisNode(FlowNode* node, QGraphicsObject* parent) :
    QtFlowNode(node, parent)
{
}
QtVisNode::~QtVisNode()
{
}
void QtVisNode::node_updated()
{
    QtFlowNode::node_updated();

    calculate_size();
    
    QPointF pin_pos = _pin->local_pos();
    if (_pin->pin_type() == FlowPin::Out)
        pin_pos.setX(_rect.width());
    else
        pin_pos.setX(0);

    _pin->set_local_pos(pin_pos);

    update();
    scene()->update();
}
void QtVisNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
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
    QFontMetrics metrics(fnt);

    QPoint title_pos(20, 20);

    fnt.setBold(true);
    painter->setFont(fnt);
    painter->setPen(style.node_title_color);

    painter->drawText(title_pos, "Image");
    
    if (!_qimage.isNull())
    {
        QImage thumb = _qimage.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter->drawImage(title_pos + QPointF(0, 10), thumb);
    }

    QtFlowPin* pin = _pins[0];
    painter->setBrush(pin->color());
    painter->setPen(pin->outline_color());
    painter->drawEllipse(pin->local_pos(), style.pin_radius, style.pin_radius);

}
void QtVisNode::create_pins()
{
    assert(_node->pins().size() == 1);

    const FlowUIStyle& style = FlowUIStyle::default_style();

    QFontMetrics metrics(style.node_font);
    
    FlowPin* pin = _node->pins()[0];
    QPoint pin_pos;
    if (pin->pin_type() == FlowPin::Out) 
        pin_pos = QPoint(_rect.width(), _rect.height() / 2);
    else
        pin_pos = QPoint(0, _rect.height() / 2);
    _pin = new QtFlowPin(this, _node->pins()[0], pin_pos);
    _pins.push_back(_pin);
}
void QtVisNode::calculate_size()
{
    assert(_node->pins().size() == 1);

    QFont fnt = FlowUIStyle::default_style().node_font;
    QFontMetrics font_metrics(fnt);
    int height = font_metrics.height() + 110; // Title
    int width = std::max(120, font_metrics.width("Image") + 25);
    _rect = QRect(0, 0, width, height);
}
void QtVisNode::show_image(const Image& image)
{
    if (!image.valid())
        return;

    _data = image.data().contiguous();
    _data = _data.cast(NPY_UINT8);

    assert(_data.ndims() <= 3);

    //assert(image.pixel_type() == image::PixelType_UInt8);

    QImage::Format fmt = QImage::Format_Invalid;

    if (_data.ndims() == 3)
    {
        // 2D image with multiple channels
        int n_channels = _data.shape()[2];
        if (n_channels == 3)
        {
            fmt = QImage::Format_RGB888;
        }
        else if (n_channels == 4)
        {
            fmt = QImage::Format_RGBA8888;
        }
    }
    else if (_data.ndims() == 2 || _data.ndims() == 1)
    {
        fmt = QImage::Format_Grayscale8;
    }

    if (fmt == QImage::Format_Invalid)
    {
        PYTHON_ERROR(TypeError, "Invalid image format");
    }

    _qimage = QImage((uint8_t*)_data.data(), _data.shape()[1], _data.shape()[0], (int)_data.strides()[0], fmt);
    
    update();
}

