#include <Core/Common.h>
#include <Core/Image/Image.h>

#include "QtVisWindow.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>

QtVisWindow::QtVisWindow(QWidget *parent) : QMainWindow(parent), _view(nullptr)
{
    setup_ui();
}
QtVisWindow::~QtVisWindow()
{
}
void QtVisWindow::set_image(const Image& image)
{
    if (!image.valid())
        return;

    NumpyArray arr = image.data().contiguous();
    assert(arr.ndims() <= 3);

    //assert(image.pixel_type() == image::PixelType_UInt8);
    
    QImage::Format fmt = QImage::Format_Invalid;
    if (image.pixel_type() == image::PixelType_UInt8)
        fmt = QImage::Format_Grayscale8;
    else if (image.pixel_type() == image::PixelType_Vec3u8)
        fmt = QImage::Format_RGB888;
    else if (image.pixel_type() == image::PixelType_Vec4u8)
        fmt = QImage::Format_RGBA8888;
    assert(fmt != QImage::Format_Invalid);
    
    QImage qimage((uint8_t*)arr.data(), arr.shape()[1], arr.shape()[0], (int)arr.strides()[0], fmt);
    QPixmap pixmap = QPixmap::fromImage(qimage);

    _view->scene()->clear();
    _view->scene()->addPixmap(pixmap);
    _view->scene()->setSceneRect(pixmap.rect());
    _view->fitInView(_view->scene()->sceneRect(), Qt::KeepAspectRatio);
}
void QtVisWindow::resizeEvent(QResizeEvent* e)
{
    _view->fitInView(_view->scene()->sceneRect(), Qt::KeepAspectRatio);
    QMainWindow::resizeEvent(e);
}
void QtVisWindow::setup_ui()
{
    setWindowIcon(QIcon(":/res/icon.ico"));

    setMinimumSize(400, 300);
    
    _view = new QGraphicsView(this);
    _view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCentralWidget(_view);

    _view->setScene(new QGraphicsScene(_view));
}
