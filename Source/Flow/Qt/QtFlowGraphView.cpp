#include <Core/Common.h>

#include "FlowGraph.h"
#include "FlowModule.h"
#include "FlowNode.h"
#include "FlowPin.h"
#include "QtFlowLink.h"
#include "QtFlowGraphScene.h"
#include "QtFlowGraphView.h"
#include "QtFlowNode.h"
#include "QtFlowPin.h"

#include <QGraphicsItem>
#include <QMenu>
#include <QMetaType>
#include <QMouseEvent>

Q_DECLARE_METATYPE(FlowNode*);

QtFlowGraphView::QtFlowGraphView(QWidget *parent)
    : QGraphicsView(parent),
    _node_menu(nullptr),
    _mode(Mode_Nothing),
    _selected_pin(nullptr),
    _temp_pin(nullptr),
    _temp_link(nullptr),
    _highlight_pin(nullptr)
{
    build_node_menu();

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(show_context_menu(const QPoint&)));

    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    QtFlowGraphScene* graph_scene = new QtFlowGraphScene(this);
    graph_scene->setSceneRect(-2500, -2500, 5000, 5000);
    set_scene(graph_scene);
    

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

QtFlowGraphView::~QtFlowGraphView()
{
}
void QtFlowGraphView::set_scene(QtFlowGraphScene* scene)
{
    _scene = scene;
    setScene(scene);
}
QtFlowGraphScene* QtFlowGraphView::scene() const
{
    return _scene;
}

void QtFlowGraphView::mousePressEvent(QMouseEvent* mouse_event)
{
    if (_scene && mouse_event->button() & Qt::MouseButton::LeftButton)
    {
        emit flow_node_selected(nullptr);

        auto scene_items = items(mouse_event->pos());
        if (scene_items.size() != 0)
        {
            for (auto& selected : scene_items)
            {
                if (selected->type() == QtFlowNode::Type/* || selected->type() == QtTerminalNode::Type*/)
                {
                    _scene->clearSelection();
                    _scene->clearFocus();

                    selected->setSelected(true);

                    emit flow_node_selected((QtBaseNode*)selected);

                    _mode = Mode_Move;

                    break;
                }
                else if (selected->type() == QtFlowPin::Type)
                {
                    _scene->clearSelection();
                    _scene->clearFocus();

                    _selected_pin = (QtFlowPin*)selected;
                    if (_temp_link)
                        delete _temp_link;

                    _temp_link = nullptr;
                    if (_selected_pin->pin_type() == FlowPin::Out)
                    {
                        _temp_pin = new QtFlowPin();
                        _temp_link = new QtFlowLink(_selected_pin, _temp_pin);
                    }
                    else
                    {
                        _temp_pin = new QtFlowPin();
                        _temp_link = new QtFlowLink(_temp_pin, _selected_pin);
                    }
                    _temp_pin->setPos(mapToScene(mouse_event->pos()));

                    _scene->addItem(_temp_link);

                    _mode = Mode_DragPin;

                    break;
                }
                else if (selected->type() == QtFlowLink::Type)
                {
                    _scene->clearSelection();
                    _scene->clearFocus();
                    selected->setSelected(true);
                    break;
                }

            }
        }
        else
        {
            _mode = Mode_Scroll;
            setDragMode(DragMode::ScrollHandDrag);
            QGraphicsView::mousePressEvent(mouse_event);

            _scene->clearSelection();
            _scene->clearFocus();
        }
    }
    else
    {
        QGraphicsView::mousePressEvent(mouse_event);
    }
    _last_mouse_pos = mouse_event->pos();
}
void QtFlowGraphView::mouseMoveEvent(QMouseEvent* mouse_event)
{
    switch (_mode)
    {
    case Mode_Move:
        for (auto& item : _scene->selectedItems())
        {
            if (item->type() == QtFlowNode::Type)// || item->type() == QtTerminalNode::Type)
            {
                QPointF delta = mapToScene(mouse_event->pos()) - mapToScene(_last_mouse_pos);
                item->setPos(item->pos() + delta);
            }
        }
        break;
    case Mode_DragPin:
    {
        if (_highlight_pin)
        {
            _highlight_pin->set_highlighted(false);
            _highlight_pin = nullptr;
        }

        auto scene_items = items(mouse_event->pos());
        for (auto& selected : scene_items)
        {
            if (selected->type() == QtFlowPin::Type)
            {
                QtFlowPin* pin = (QtFlowPin*)selected;

                if (pin != _selected_pin &&
                    pin->pin_type() != _selected_pin->pin_type())
                {
                    // Check if pins match
                    //FlowPin* in_pin = (_selected_pin->pin_type() == FlowPin::In) ? _selected_pin->pin() : pin->pin();
                    //FlowPin* out_pin = (_selected_pin->pin_type() == FlowPin::Out) ? _selected_pin->pin() : pin->pin();

                    // Input pin determines supported types
                    //if (out_pin->type()->is_type(in_pin->type()))
                    //{
                    _highlight_pin = pin;
                    _highlight_pin->set_highlighted(true);
                    //}
                }
            }
        }

        _temp_pin->setPos(mapToScene(mouse_event->pos()));
        _temp_pin->update();
        _temp_link->update();

        break;
    }
    case Mode_Scroll:
    {
        QGraphicsView::mouseMoveEvent(mouse_event);
        break;
    }
    default:
        QGraphicsView::mouseMoveEvent(mouse_event);
    };
    _last_mouse_pos = mouse_event->pos();
}
void QtFlowGraphView::mouseReleaseEvent(QMouseEvent* mouse_event)
{
    switch (_mode)
    {
    case Mode_DragPin:
        if (_temp_pin && _highlight_pin && _selected_pin)
        {
            _temp_link->set_pin(_highlight_pin);
            _temp_link->set_pin(_selected_pin);

            if (_scene->try_add_link(_temp_link))
            {
                _highlight_pin->set_highlighted(false);
                _highlight_pin = nullptr;
                _temp_link = nullptr;
            }
        }

        if (_temp_link)
        {
            delete _temp_link;
            _temp_link = nullptr;
        }

        if (_temp_pin)
        {
            delete _temp_pin;
            _temp_pin = nullptr;
        }
        _selected_pin = nullptr;

        break;
    case Mode_Scroll:
    {
        setDragMode(DragMode::NoDrag);
        QGraphicsView::mouseMoveEvent(mouse_event);
        break;
    }
    default:
        QGraphicsView::mouseMoveEvent(mouse_event);
    };
    _last_mouse_pos = mouse_event->pos();

    _mode = Mode_Nothing;
}
void QtFlowGraphView::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Delete)
    {
        for (auto& item : _scene->selectedItems())
        {
            if (item->type() == QtFlowNode::Type)// || item->type() == QtTerminalNode::Type)
            {
                QtFlowNode* node = (QtFlowNode*)item;
                _scene->remove_node(node);
            }
            else if (item->type() == QtFlowLink::Type)
            {
                QtFlowLink* link = (QtFlowLink*)item;
                _scene->remove_link(link);
                delete link;
            }
        }
        _scene->clearSelection();
        _scene->update();
    }

    QGraphicsView::keyPressEvent(e);
}
void QtFlowGraphView::wheelEvent(QWheelEvent *e)
{
    qreal zoom = e->delta()*0.0005;
    
    QTransform t = transform();
    t.scale(1 + zoom, 1 + zoom);
    setTransform(t);
}

void QtFlowGraphView::drawBackground(QPainter * painter, const QRectF & rect)
{
    const QRectF nrect = rect.normalized();
    painter->save();
    painter->setPen(QPen(Qt::lightGray, 1));
    int l = int(nrect.left());
    l -= (l % 10);

    int r = int(nrect.right());
    r -= (r % 10);
    if (r < int(nrect.right()))
        r += 10;

    int t = int(nrect.top());
    t -= (t % 10);

    int b = int(nrect.bottom());
    b -= (b % 10);
    if (b < int(nrect.bottom()))
        b += 10;

    for (int x = l; x <= r; x += 10)
        for (int y = t; y <= b; y += 10)
            painter->drawPoint(x, y);

    painter->restore();
}
void QtFlowGraphView::build_node_menu()
{
    std::vector<FlowNode*> nodes = FlowModule::instance().node_templates();
    if (!nodes.empty())
    {
        _node_menu = new QMenu(this);
        std::map<std::string, QMenu*> sub_menus;
        sub_menus[""] = _node_menu;

        std::sort(nodes.begin(), nodes.end(), [](FlowNode* x, FlowNode* y)
        {
            int c = strcmp(x->category(), y->category());
            if (c < 0)
                return true;
            else if (c > 0)
                return false;

            return strcmp(x->title(), y->title()) < 0;
        });

        // Build menus
        for (auto& node : nodes)
        {
            if (sub_menus.find(node->category()) == sub_menus.end())
            {
                std::vector<std::string> elems;

                std::stringstream ss(node->category());
                std::string item;
                while (std::getline(ss, item, '/'))
                {
                    elems.push_back(item);
                }


                QMenu* target_menu = _node_menu;
                std::string menu_str = "";
                for (int i = 0; i < int(elems.size()); ++i)
                {
                    if (menu_str != "")
                        menu_str += "/";
                    menu_str += elems[i];
                    auto it = sub_menus.find(menu_str);
                    if (it == sub_menus.end())
                    {
                        sub_menus[menu_str] = new QMenu(QString::fromStdString(elems[i]), target_menu);
                        target_menu->addMenu(sub_menus[menu_str]);
                    }
                    target_menu = sub_menus[menu_str];
                }
            }
        }

        // Insert nodes
        for (auto& node : nodes)
        {
            QAction* node_action = new QAction(QString::fromStdString(node->title()), _node_menu);
            node_action->setData(QVariant::fromValue<FlowNode*>(node));

            QMenu* target_menu = sub_menus[node->category()];
            target_menu->addAction(node_action);

        }
    }
}

void QtFlowGraphView::show_context_menu(const QPoint& pt)
{
    if (!_scene)
        return;

    auto scene_items = items(pt);
    if (scene_items.size())
        return;

    if (_node_menu)
    {
        QPoint global_pt = mapToGlobal(pt);

        QAction* action = _node_menu->exec(global_pt);
        if (action)
        {
            FlowNode* template_node = action->data().value<FlowNode*>();
            if (template_node)
            {
                FlowNode* node = object_cast<FlowNode>(object_clone(template_node));
                node->set_node_id(guid::create_guid());
                _scene->create_node(node, mapToScene(pt));
            }
        }
    }
}
void QtFlowGraphView::node_template_added(FlowNode* )
{
    // Rebuild node menu
    build_node_menu();
}
void QtFlowGraphView::node_template_removed(FlowNode* )
{
    // Rebuild node menu
    build_node_menu();
}
