#ifndef __FLOW_GRAPH_NOTE_H__
#define __FLOW_GRAPH_NOTE_H__

#include <Core/Python/BaseObject.h>
#include <Core/Image/Vec2.h>

#include "API.h"

class FlowGraph;
class FLOW_API GraphNote : public python::BaseObject
{
    PYTHON_OBJECT(GraphNote, python::BaseObject);

public:
    GraphNote();
    GraphNote(const GraphNote& other);
    ~GraphNote();

    void set_graph(FlowGraph* graph);

    Guid id() const;
    void set_id(const Guid& id);

    Vec2i position() const;
    void set_position(const Vec2i& pos);

    const char* text() const;
    void set_text(const char* text);

private:
    FlowGraph* _graph;
    Guid _id;

    Vec2i _position;
    std::string _text;
};

#endif // __FLOW_GRAPH_NOTE_H__
