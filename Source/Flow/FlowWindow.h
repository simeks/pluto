#ifndef __FLOW_WINDOW_H__
#define __FLOW_WINDOW_H__

#include "API.h"
#include <Core/Object/Object.h>
#include "Qt/QtFlowWindow.h"

class FlowGraph;
class QtFlowWindow;
class FLOW_API FlowWindow : public Object
{
    DECLARE_OBJECT(FlowWindow, Object);

public:
    DECLARE_OBJECT_CONSTRUCTOR(FlowWindow);
    ~FlowWindow();

    void object_init();
    void object_init(QtFlowWindow* window);

    void show();
    void close();

    FlowGraph* load(const char* file);
    void save(const char* file);

    FlowGraph* graph();
    void set_graph(FlowGraph* graph);
    void clear();

    Dict run(const Tuple& args, const Dict& kw);

private:
    QtFlowWindow* _window;

};

#endif // __FLOW_WINDOW_H__
