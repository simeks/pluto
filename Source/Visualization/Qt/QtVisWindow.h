#ifndef __QT_VIS_WINDOW_H__
#define __QT_VIS_WINDOW_H__

#include "API.h"
#include <QDialog>
#include <QMainWindow>

#include <Core/Python/NumPy.h>

class Image;
class QGraphicsView;
class VIS_API QtVisWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtVisWindow(QWidget *parent = nullptr);
    ~QtVisWindow();


public slots:
    void set_image(const Image& img);
    void set_image(const NumpyArray& img);

protected:
    virtual void resizeEvent(QResizeEvent* e) OVERRIDE;
    virtual void showEvent(QShowEvent* e) OVERRIDE;

private:
    void setup_ui();

    NumpyArray _data;
    QGraphicsView* _view;
};

#endif // __QT_VIS_WINDOW_H__
