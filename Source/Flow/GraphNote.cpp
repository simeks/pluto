#include <Core/Common.h>

#include "GraphNote.h"


PLUTO_OBJECT_IMPL(GraphNote, "Note")
{
    cls.def_init<GraphNote>();
}

GraphNote::GraphNote() :
    _graph(nullptr),
    _id(guid::create_guid())
{
}
GraphNote::GraphNote(const GraphNote& other) : 
    _graph(other._graph),
    _id(_id),
    _position(other._position),
    _text(other._text)
{
}
GraphNote::~GraphNote()
{
}
void GraphNote::set_graph(FlowGraph* graph)
{
    _graph = graph;
}
Guid GraphNote::id() const
{
    return _id;
}
void GraphNote::set_id(const Guid& id)
{
    _id = id;
}
Vec2i GraphNote::position() const
{
    return _position;
}
void GraphNote::set_position(const Vec2i& pos)
{
    _position = pos;
}
const char* GraphNote::text() const
{
    return _text.c_str();
}
void GraphNote::set_text(const char* text)
{
    _text = text;
}

