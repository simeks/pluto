#include <Core/Common.h>

#include "FlowGraph.h"
#include "FlowNode.h"
#include "QtFlowGraphScene.h"
#include "QtFlowLink.h"
#include "QtFlowNode.h"
#include "QtFlowPin.h"

#include <QtGui>
#include <QGraphicsSceneMouseEvent>

QtFlowGraphScene::QtFlowGraphScene(QObject *parent)
    : QGraphicsScene(parent)
{
    _flow_graph = object_new<FlowGraph>();
}
QtFlowGraphScene::QtFlowGraphScene(FlowGraph* graph, QObject *parent)
    : QGraphicsScene(parent)
{
    set_graph(graph);
}
QtFlowGraphScene::~QtFlowGraphScene()
{
    clear_scene();
}

void QtFlowGraphScene::create_node(FlowNode* node, const QPointF& pos)
{
    QtBaseNode* ui_node = nullptr;
    //if (node->is_a(TerminalNode::static_class()))
    //{
    //    ui_node = new QtTerminalNode(node);
    //}
    //else
    {
        ui_node = new QtFlowNode(node);
    }
    ui_node->setPos(pos);
    addItem(ui_node);

    _flow_graph->add_node(node);
    _nodes[node->node_id()] = ui_node;
}
void QtFlowGraphScene::add_node(QtBaseNode* node)
{
    addItem(node);
    _flow_graph->add_node(node->node());
    _nodes[node->node_id()] = node;
}
void QtFlowGraphScene::remove_node(QtBaseNode* node)
{
    auto it = _nodes.find(node->node_id());
    if (it != _nodes.end())
        _nodes.erase(it);

    _flow_graph->remove_node(node->node());

    remove_links(node);
    removeItem(node);
    delete node;
}
bool QtFlowGraphScene::try_add_link(QtFlowLink* link)
{
    QtFlowPin* a = link->start_pin();
    QtFlowPin* b = link->end_pin();
    if (!a || !b)
        return false;

    if (_flow_graph->try_add_link(a->pin(), b->pin()))
    {
        _links.push_back(link);
        return true;
    }
    return false;
}
void QtFlowGraphScene::remove_link(QtFlowLink* link)
{
    _flow_graph->remove_link(link->start_pin()->pin(), link->end_pin()->pin());
    _links.erase(std::remove(_links.begin(), _links.end(), link), _links.end());
    removeItem(link);
}
void QtFlowGraphScene::remove_links(QtBaseNode* node)
{
    for (auto p : node->pins())
    {
        remove_links(p);
    }
}
void QtFlowGraphScene::remove_links(QtFlowPin* pin)
{
    auto it = _links.begin();
    while (it != _links.end())
    {
        if ((*it)->start_pin() == pin || (*it)->end_pin() == pin)
        {
            removeItem(*it);
            it = _links.erase(it);
        }
        else
            ++it;
    }
}
void QtFlowGraphScene::clear_scene()
{
    if (_flow_graph)
        _flow_graph->release();

    _nodes.clear();
    _links.clear();

    clear();

    emit scene_cleared();
}
FlowGraph* QtFlowGraphScene::graph() const
{
    return _flow_graph;
}
void QtFlowGraphScene::set_graph(FlowGraph* graph)
{
    clear_scene();
    _flow_graph = graph;

    for (auto n : graph->nodes())
    {
        FlowNode* node = n.second;

        QtFlowNode* ui_node = new QtFlowNode(node);
        ui_node->setPos(node->ui_pos().x, node->ui_pos().y);
        addItem(ui_node);

        _nodes[node->node_id()] = ui_node;
    }

    for (const FlowGraph::Link& l : graph->links())
    {
        QtBaseNode* begin_node = _nodes[l.first->owner()->node_id()];
        QtBaseNode* end_node = _nodes[l.second->owner()->node_id()];

        QtFlowPin* begin_pin = begin_node->pins()[l.first->pin_id()];
        QtFlowPin* end_pin = end_node->pins()[l.second->pin_id()];

        QtFlowLink* link = new QtFlowLink(begin_pin, end_pin);
        addItem(link);
        _links.push_back(link);
    }
}