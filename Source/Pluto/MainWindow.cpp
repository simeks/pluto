#include <Core/Common.h>

#include "MainWindow.h"
#include "Console/ConsoleWidget.h"

#include <QCoreApplication>
#include <QSettings>
#include <QGridLayout>

#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
_console_widget(nullptr)
{
    setMinimumSize(400, 300);

    QCoreApplication::setOrganizationName("Simon");
    QCoreApplication::setApplicationName("Pluto");

    QSettings settings;

    settings.beginGroup("mainwindow");

    restoreGeometry(settings.value("geometry", saveGeometry()).toByteArray());
    restoreState(settings.value("savestate", saveState()).toByteArray());
    move(settings.value("pos", pos()).toPoint());
    resize(settings.value("size", size()).toSize());
    if (settings.value("maximized", isMaximized()).toBool())
        showMaximized();

    settings.endGroup();

    setup_ui();
}

MainWindow::~MainWindow()
{
    QSettings settings;

    settings.beginGroup("mainwindow");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("savestate", saveState());
    settings.setValue("maximized", isMaximized());
    if (!isMaximized()) {
        settings.setValue("pos", pos());
        settings.setValue("size", size());
    }

    settings.endGroup();
}

void MainWindow::setup_ui()
{
    QAction* action_exit = new QAction(this);
    action_exit->setObjectName("exit");

    QMenuBar* menu_bar = new QMenuBar(this);
    setMenuBar(menu_bar);

    QMenu* menu_file = new QMenu(menu_bar);
    menu_bar->addAction(menu_file->menuAction());

    menu_file->addSeparator();
    menu_file->addAction(action_exit);

    setWindowTitle("MainWindow");
    menu_file->setTitle("File");
    action_exit->setText("Exit");

    QMetaObject::connectSlotsByName(this);

    _console_widget = new ConsoleWidget(this);
    setCentralWidget(_console_widget);
    _console_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MainWindow::on_exit_triggered()
{
    close();
}
