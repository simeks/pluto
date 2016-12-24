#ifndef __QT_FLOW_DIAGRAM_SCENE_H__
#define __QT_FLOW_DIAGRAM_SCENE_H__

#include <Core/Common.h>
#include <QGraphicsScene>

class FlowGraph;
class FlowNode;
class QtBaseNode;
class QtFlowLink;
class QtFlowPin;
class QtFlowGraphScene : public QGraphicsScene
{
    Q_OBJECT

public:
    QtFlowGraphScene(QObject *parent = nullptr);
    QtFlowGraphScene(FlowGraph* graph, QObject *parent = nullptr);
    ~QtFlowGraphScene();

    void create_node(FlowNode* node, const QPointF& pos);
    void add_node(QtBaseNode* node);
    void remove_node(QtBaseNode* node);

    bool try_add_link(QtFlowLink* link);
    void remove_link(QtFlowLink* link);

    /// Removes all links associated with node
    void remove_links(QtBaseNode* node);
    /// Removes all links associated with pin
    void remove_links(QtFlowPin* pin);

    void clear_scene();

    FlowGraph* graph() const;
    void set_graph(FlowGraph* graph);
    
private:
    FlowGraph* _flow_graph;

    std::map<Guid, QtBaseNode*> _nodes;
    std::vector<QtFlowLink*> _links;

signals:
    void scene_cleared();
};


#endif // __QT_FLOW_DIAGRAM_SCENE_H__

