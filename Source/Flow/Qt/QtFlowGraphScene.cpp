#include <Core/Common.h>

#include "FlowGraph.h"
#include "FlowNode.h"
#include "FlowPin.h"
#include "QtFlowGraphScene.h"
#include "QtFlowLink.h"
#include "QtFlowNode.h"
#include "QtFlowPin.h"

#include "Nodes/QtConstantNode.h"
#include "Nodes/QtPrintNode.h"
#include "Nodes/QtVariableNode.h"

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
    if (_flow_graph)
        _flow_graph->release();

    _nodes.clear();
    _links.clear();

    clear();
}

void QtFlowGraphScene::create_node(FlowNode* node, const QPointF& pos)
{
    QtFlowNode* ui_node = _create_node(node);
    ui_node->setPos(pos);
    addItem(ui_node);

    _flow_graph->add_node(node);
    _nodes[node->node_id()] = ui_node;
}
void QtFlowGraphScene::add_node(QtFlowNode* node)
{
    addItem(node);
    _flow_graph->add_node(node->node());
    _nodes[node->node_id()] = node;
}
void QtFlowGraphScene::remove_node(QtFlowNode* node)
{
    auto it = _nodes.find(node->node_id());
    if (it != _nodes.end())
        _nodes.erase(it);

    _flow_graph->remove_node(node->node());

    remove_links(node);
    removeItem(node);
}
bool QtFlowGraphScene::try_add_link(QtFlowLink* link)
{
    QtFlowPin* a = link->start();
    QtFlowPin* b = link->end();
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
    _flow_graph->remove_link(link->start()->pin(), link->end()->pin());
    _links.erase(std::remove(_links.begin(), _links.end(), link), _links.end());
    removeItem(link);
}
void QtFlowGraphScene::remove_links(QtFlowNode* node)
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
        if ((*it)->start() == pin || (*it)->end() == pin)
        {
            removeItem(*it);
            it = _links.erase(it);
        }
        else
            ++it;
    }
}
void QtFlowGraphScene::find_links(QtFlowPin* pin, std::vector<QtFlowLink*>& links) const
{
    for (auto l : _links)
    {
        if (l->start() == pin || l->end() == pin)
            links.push_back(l);
    }
}
QtFlowLink* QtFlowGraphScene::find_link(QtFlowPin* pin) const
{
    for (auto l : _links)
    {
        if (l->start() == pin || l->end() == pin)
            return l;
    }
    return nullptr;
}
void QtFlowGraphScene::new_graph()
{
    clear_scene();

    if (_flow_graph)
        _flow_graph->release();

    _flow_graph = object_new<FlowGraph>();
}
FlowGraph* QtFlowGraphScene::graph() const
{
    return _flow_graph;
}
void QtFlowGraphScene::set_graph(FlowGraph* graph)
{
    clear_scene();
    _flow_graph = graph;

    /// Links pointing from first pin to second pin
    typedef std::pair<FlowPin*, FlowPin*> Link;
    std::vector<Link> links;
    for (auto n : graph->nodes())
    {
        FlowNode* node = n.second;
        QtFlowNode* ui_node = _create_node(node);
        ui_node->setPos(node->ui_pos().x, node->ui_pos().y);
        addItem(ui_node);

        _nodes[node->node_id()] = ui_node;

        for (auto outpin : node->pins())
        {
            if (outpin->pin_type() == FlowPin::Out)
            {
                for (auto inpin : outpin->links())
                {
                    links.push_back(Link(outpin, inpin));
                }
            }
        }
    }


    for (const Link& l : links)
    {
        QtFlowNode* begin_node = _nodes[l.first->owner()->node_id()];
        QtFlowNode* end_node = _nodes[l.second->owner()->node_id()];

        QtFlowPin* begin_pin = begin_node->pins()[l.first->pin_id()];
        QtFlowPin* end_pin = end_node->pins()[l.second->pin_id()];

        QtFlowLink* link = new QtFlowLink(begin_pin, end_pin);
        addItem(link);
        _links.push_back(link);
    }
}
QtFlowNode* QtFlowGraphScene::node(const Guid& id) const
{
    auto it = _nodes.find(id);
    if (it != _nodes.end())
        return it->second;

    return nullptr;
}
void QtFlowGraphScene::clear_scene()
{
    _nodes.clear();
    _links.clear();

    clear();
}
QtFlowNode* QtFlowGraphScene::_create_node(FlowNode* node)
{
    QtFlowNode* n = nullptr;

    QString cls = node->node_class();
    if (cls == "flow.Constant")
    {
        n = new QtConstantNode(node);
    }
    else if (cls == "flow.Print")
    {
        n = new QtPrintNode(node);
    }
    else if (cls == "flow.Variable")
    {
        n = new QtVariableNode(node);
    }
    else
    {
        n = new QtFlowNode(node);
    }
    n->setup();
    return n;
}
