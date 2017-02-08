#include <Core/Common.h>
#include <Core/Image/Image.h>
#include <Core/Python/PythonCommon.h>

#include "Image.h"
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

    _data = format_data(image.data(), image.data().type() == NPY_BOOL ? visualization::ImageType_Bool : visualization::ImageType_Unknown);
    QImage qimage = convert_to_qimage(_data);
    if (qimage.isNull())
    {
        PYTHON_ERROR(TypeError, "Invalid image format");
    }

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
