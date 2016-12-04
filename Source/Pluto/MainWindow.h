#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QMainWindow>

class ConsoleWidget;
class QtFlowDiagramView;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setup_ui();

    ConsoleWidget* _console_widget;

private slots:
    void on_exit_triggered();

};

#endif // __MAIN_WINDOW_H__
