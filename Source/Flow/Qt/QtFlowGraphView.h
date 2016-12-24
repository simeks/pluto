#ifndef __QT_FLOW_DIAGRAM_VIEW_H__
#define __QT_FLOW_DIAGRAM_VIEW_H__

#include <QGraphicsView>

class FlowNode;
class QMenu;
class QtBaseNode;
class QtFlowLink;
class QtFlowGraphScene;
class QtFlowPin;
class QtFlowGraphView : public QGraphicsView
{
    Q_OBJECT

public:
    QtFlowGraphView(QWidget *parent = nullptr);
    ~QtFlowGraphView();

    void set_scene(QtFlowGraphScene* scene);
    QtFlowGraphScene* scene() const;

public slots:
    void node_template_added(FlowNode* node);
    void node_template_removed(FlowNode* node);

protected:
    void mousePressEvent(QMouseEvent* mouse_event);
    void mouseMoveEvent(QMouseEvent* mouse_event);
    void mouseReleaseEvent(QMouseEvent* mouse_event);
    void keyPressEvent(QKeyEvent *e);
    void wheelEvent(QWheelEvent *e);

    void drawBackground(QPainter * painter, const QRectF & rect);
private:
    enum Mode
    {
        Mode_Nothing,
        Mode_Move,
        Mode_DragPin,
        Mode_Scroll
    };

    QMenu* _node_menu;

    QtFlowGraphScene* _scene;

    QPoint _last_mouse_pos;

    Mode _mode;

    QtFlowPin* _selected_pin;
    QtFlowPin* _temp_pin;
    QtFlowLink* _temp_link;
    QtFlowPin* _highlight_pin;

    void build_node_menu();

private slots:
    void show_context_menu(const QPoint&);

signals:
    void flow_node_selected(QtBaseNode* node);

};


#endif // __QT_FLOW_DIAGRAM_VIEW_H__

