#include <Core/Common.h>

#include "Console/ConsoleWidget.h"
#include "MainWindow.h"
#include "PlutoApplication.h"

#include <QCoreApplication>
#include <QGridLayout>
#include <QMessageBox>
#include <QSettings>

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
    QMenuBar* menu_bar = new QMenuBar(this);
    setMenuBar(menu_bar);

    {
        QMenu* menu_file = new QMenu("File", menu_bar);
        menu_bar->addAction(menu_file->menuAction());

        QAction* action_new = new QAction(tr("New"), this);
        action_new->setShortcuts(QKeySequence::New);
        action_new->setStatusTip("Creates a new graph");
        action_new->setDisabled(true);

        QAction* action_open = new QAction(tr("Open"), this);
        action_open->setShortcuts(QKeySequence::Open);
        action_open->setStatusTip("Opens an existing graph");
        action_open->setDisabled(true);

        QAction* action_exit = new QAction(tr("Exit"), this);
        action_exit->setShortcuts(QKeySequence::Quit);
        action_exit->setStatusTip("Closes this window");
        connect(action_exit, &QAction::triggered, this, &MainWindow::on_exit_triggered);

        menu_file->addAction(action_new);
        menu_file->addAction(action_open);
        menu_file->addSeparator();
        menu_file->addAction(action_exit);
    }
    {
        QMenu* menu_help = new QMenu("Help", menu_bar);
        menu_bar->addAction(menu_help->menuAction());

        QAction* action_about = new QAction(tr("About Pluto"), this);
        menu_help->addAction(action_about);
        connect(action_about, &QAction::triggered, this, &MainWindow::about);
    }

    setWindowTitle("Plumbing Toolbox");

    _console_widget = new ConsoleWidget(PlutoApplication::instance().kernel(), this);
    setCentralWidget(_console_widget);
    _console_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
void MainWindow::on_exit_triggered()
{
    close();
}
void MainWindow::about()
{
    QMessageBox::about(this, tr("About Pluto"),
        tr("Developed by Simon Ekström (simon.ekstrom@surgsci.uu.se)"));
}
