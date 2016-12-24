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
    FlowWindow();
    FlowWindow(QtFlowWindow* window);
    ~FlowWindow();

    void show();
    void close();

    FlowGraph* load(const char* file);
    void save(const char* file);

    FlowGraph* graph();
    void set_graph(FlowGraph* graph);
    void clear();

    FlowWindow(const FlowWindow& other);
    FlowWindow& operator=(const FlowWindow& other);

private:
    QtFlowWindow* _window;

};

#endif // __FLOW_WINDOW_H__
