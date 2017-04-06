#include <Core/Common.h>

#include "FlowGraph.h"
#include "FlowWindow.h"
#include "Qt/QtFlowWindow.h"


python::Class FlowWindow::python_class()
{
    static python::Class cls;
    if (cls.is_none())
    {
        cls = python::make_class<FlowWindow>("Window");
        cls.def("show", &FlowWindow::show);
        cls.def("close", &FlowWindow::close);
        cls.def("load", &FlowWindow::load);
        cls.def("save", &FlowWindow::save);
        cls.def("graph", &FlowWindow::graph);
        cls.def("set_graph", &FlowWindow::set_graph);
        cls.def("run", &FlowWindow::run);
        cls.def("resume", &FlowWindow::resume);
    }
    return cls;
}

FlowWindow::FlowWindow(QtFlowWindow* window) : _window(window)
{
}
FlowWindow::~FlowWindow()
{
}
void FlowWindow::show()
{
    QMetaObject::invokeMethod(_window, "show");
}
void FlowWindow::close()
{
    QMetaObject::invokeMethod(_window, "close");
}
FlowGraph* FlowWindow::load(const char* file)
{
    QMetaObject::invokeMethod(_window, "load_graph", Qt::BlockingQueuedConnection, Q_ARG(QString, file));
    return graph();
}
void FlowWindow::save(const char* file)
{
    QMetaObject::invokeMethod(_window, "save_graph", Qt::BlockingQueuedConnection, Q_ARG(QString, file));
}
FlowGraph* FlowWindow::graph()
{
    FlowGraph* g = _window->graph();
    return clone_object(g);
}
void FlowWindow::set_graph(FlowGraph* graph)
{
    emit _window->set_graph(clone_object(graph));
}
void FlowWindow::clear()
{
    emit _window->clear_graph();
}
Dict FlowWindow::run(const Tuple& args, const Dict& kw)
{
    if (args.size() != 0)
        PYTHON_ERROR(PyExc_ValueError, "run takes only keyword arguments");

    if (_window->run_pending())
        _window->reset_run();

    return _window->run_graph(args, kw);
}
Dict FlowWindow::resume()
{
    if (_window->run_pending())
        return _window->run_graph(Tuple(), Dict());
    PYTHON_ERROR(PyExc_ValueError, "no run pending");
}

