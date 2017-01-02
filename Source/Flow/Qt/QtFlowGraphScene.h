#ifndef __QT_FLOW_DIAGRAM_SCENE_H__
#define __QT_FLOW_DIAGRAM_SCENE_H__

#include <Core/Common.h>
#include <QGraphicsScene>

class FlowGraph;
class FlowNode;
class QtFlowNode;
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
    void add_node(QtFlowNode* node);
    void remove_node(QtFlowNode* node);

    bool try_add_link(QtFlowLink* link);
    void remove_link(QtFlowLink* link);

    /// Removes all links associated with node
    void remove_links(QtFlowNode* node);
    /// Removes all links associated with pin
    void remove_links(QtFlowPin* pin);

    void new_graph();

    FlowGraph* graph() const;
    void set_graph(FlowGraph* graph);
    
private:
    void clear_scene();

    FlowGraph* _flow_graph;

    std::map<Guid, QtFlowNode*> _nodes;
    std::vector<QtFlowLink*> _links;

};


#endif // __QT_FLOW_DIAGRAM_SCENE_H__

