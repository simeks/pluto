#include <Core/Common.h>
#include <Core/Image/ImageObject.h>

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
void QtVisWindow::set_image(ImageObject* img)
{
    if (!img)
        return;

    img->addref();

    Image image = img->image();
    assert(image.ndims() == 2);
    //assert(image.pixel_type() == image::PixelType_UInt8);
    
    QImage::Format fmt = QImage::Format_Invalid;
    if (image.pixel_type() == image::PixelType_UInt8)
        fmt = QImage::Format_Grayscale8;
    else if (image.pixel_type() == image::PixelType_Vec3u8)
        fmt = QImage::Format_RGB888;
    else if (image.pixel_type() == image::PixelType_Vec4u8)
        fmt = QImage::Format_RGBA8888;
    assert(fmt != QImage::Format_Invalid);
    
    QImage qimage(image.ptr(), image.size().x, image.size().y, (int)(image.size().x * image.step()[0]), fmt);
    QPixmap pixmap = QPixmap::fromImage(qimage);

    _view->scene()->clear();
    _view->scene()->addPixmap(pixmap);
    _view->scene()->setSceneRect(pixmap.rect());
}
void QtVisWindow::setup_ui()
{
    setWindowIcon(QIcon(":/res/icon.ico"));

    setMinimumSize(800, 600);
    
    _view = new QGraphicsView(this);
    _view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCentralWidget(_view);

    _view->setScene(new QGraphicsScene(_view));
}
