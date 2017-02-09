#include <Core/Common.h>

#include "FlowGraph.h"
#include "FlowModule.h"
#include "FlowNode.h"
#include "FlowPin.h"
#include "QtFlowGraphScene.h"
#include "QtFlowLink.h"
#include "QtFlowNode.h"
#include "QtFlowPin.h"
#include "QtFlowUI.h"
#include "QtNoteItem.h"

#include "Nodes/QtConstantNode.h"
#include "Nodes/QtGraphInputNode.h"
#include "Nodes/QtGraphOutputNode.h"
#include "Nodes/QtPrintNode.h"
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
    _notes.clear();

    clear();
}

void QtFlowGraphScene::add_node(QtFlowNode* node)
{
    if (_nodes.find(node->node_id()) != _nodes.end())
        return;

    addItem(node);
    _flow_graph->add_node(node->node());
    _nodes[node->node_id()] = node;
}
void QtFlowGraphScene::remove_node(QtFlowNode* node)
{
    auto it = _nodes.find(node->node_id());
    if (it != _nodes.end())
        _nodes.erase(it);

    remove_links(node);
    removeItem(node);

    _flow_graph->remove_node(node->node());
}
void QtFlowGraphScene::add_note(QtNoteItem* note)
{
    if (std::find(_notes.begin(), _notes.end(), note) != _notes.end())
        return;

    addItem(note);
    _flow_graph->add_note(note->note());
    _notes.push_back(note);
}
void QtFlowGraphScene::remove_note(QtNoteItem* note)
{
    auto it = std::find(_notes.begin(), _notes.end(), note);
    if (it != _notes.end())
        _notes.erase(it);

    removeItem(note);

    _flow_graph->remove_note(note->note());
}
void QtFlowGraphScene::node_template_reloaded(FlowNode* tpl)
{
    _flow_graph->reload(tpl->node_class());
    set_graph(_flow_graph);
}
bool QtFlowGraphScene::try_add_link(QtFlowLink* link)
{
    QtFlowPin* a = link->start();
    QtFlowPin* b = link->end();
    if (!a || !b)
        return false;

    if (std::find(_links.begin(), _links.end(), link) != _links.end())
        return true;

    if (_flow_graph->try_add_link(a->pin(), b->pin()))
    {
        _links.push_back(link);
        if (link->scene() != this)
            addItem(link);

        a->owner()->on_pin_linked(a);
        b->owner()->on_pin_linked(b);

        return true;
    }
    return false;
}
void QtFlowGraphScene::remove_link(QtFlowLink* link)
{
    if (std::find(_links.begin(), _links.end(), link) == _links.end())
        return;

    QtFlowPin* start = link->start();
    QtFlowPin* end = link->end();

    _flow_graph->remove_link(link->start()->pin(), link->end()->pin());
    
    _links.erase(std::remove(_links.begin(), _links.end(), link), _links.end());
    removeItem(link);
    
    start->owner()->on_pin_unlinked(start);
    end->owner()->on_pin_unlinked(end);
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
            QtFlowPin* start = (*it)->start();
            QtFlowPin* end = (*it)->end();

            _flow_graph->remove_link(start->pin(), end->pin());

            it = _links.erase(it);
            removeItem(*it);

            start->owner()->on_pin_unlinked(start);
            end->owner()->on_pin_unlinked(end);
        }
        else
            ++it;
    }
}
void QtFlowGraphScene::find_links(QtFlowNode* node, std::vector<QtFlowLink*>& links) const
{
    for (auto p : node->pins())
    {
        find_links(p, links);
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
    std::map<Guid, QtFlowNode*> old_nodes = _nodes;

    clear_scene();
    graph->addref();
    _flow_graph->release();
    _flow_graph = graph;

    QtFlowUI* ui = FlowModule::instance().ui();

    /// Links pointing from first pin to second pin
    typedef std::pair<FlowPin*, FlowPin*> Link;
    std::vector<Link> links;
    for (auto& n : graph->nodes())
    {
        FlowNode* node = n.second;
        QtFlowNode* ui_node = ui->create_ui_node(node);

        auto old_node = old_nodes.find(node->node_id());
        if (old_node != old_nodes.end())
            ui_node->set_status(old_node->second->status());

        ui_node->move_node(QPointF(node->ui_pos().x, node->ui_pos().y));
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

        QtFlowPin* begin_pin = begin_node->pin(l.first->name());
        QtFlowPin* end_pin = end_node->pin(l.second->name());

        QtFlowLink* link = new QtFlowLink(begin_pin, end_pin);
        addItem(link);
        _links.push_back(link);
    }

    for (auto& n : graph->notes())
    {
        QtNoteItem* note_item = new QtNoteItem(n.second);
        addItem(note_item);

        _notes.push_back(note_item);
    }
}
QtFlowNode* QtFlowGraphScene::node(const Guid& id) const
{
    auto it = _nodes.find(id);
    if (it != _nodes.end())
        return it->second;

    return nullptr;
}
const std::vector<QtNoteItem*>& QtFlowGraphScene::notes() const
{
    return _notes;
}
void QtFlowGraphScene::clear_scene()
{
    _nodes.clear();
    _links.clear();
    _notes.clear();

    clear();
}
