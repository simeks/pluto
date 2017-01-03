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
#include "Style.h"

#include <QGraphicsItem>
#include <QMenu>
#include <QMetaType>
#include <QMouseEvent>

Q_DECLARE_METATYPE(FlowNode*);

QtFlowGraphView::QtFlowGraphView(QWidget *parent)
    : QGraphicsView(parent),
    _node_menu(nullptr),
    _mode(Mode_Nothing),
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
    setCacheMode(QGraphicsView::CacheBackground);

    const FlowUIStyle& style = FlowUIStyle::default_style();
    setBackgroundBrush(style.background_color);
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
            for (auto& item : scene_items)
            {
                if (item->type() == QtFlowNode::Type)
                {
                    QtFlowNode* node = (QtFlowNode*)item;
                    int pin_id = node->check_pin(node->mapFromScene(mapToScene(mouse_event->pos())));
                    if (pin_id != -1)
                    {
                        _scene->clearSelection();
                        _scene->clearFocus();

                        if (_temp_link)
                            delete _temp_link;
                        _temp_link = nullptr;

                        QtFlowPin* pin = node->pin(pin_id);
                        if (pin->pin_type() == FlowPin::In && pin->is_linked())
                        {
                            QtFlowLink* link = _scene->find_link(pin);
                            assert(link);

                            _scene->remove_link(link);

                            link->unset_pin(FlowPin::In);
                            _temp_link = link;
                        }
                        else
                        {
                            if (pin->pin_type() == FlowPin::Out)
                            {
                                _temp_link = new QtFlowLink(pin, nullptr);
                            }
                            else
                            {
                                _temp_link = new QtFlowLink(nullptr, pin);
                            }
                        }
                        _temp_link->move_free_end(mapToScene(mouse_event->pos()));
                        _scene->addItem(_temp_link);

                        _mode = Mode_DragPin;
                        break;
                    }
                    else
                    {
                        _scene->clearSelection();
                        _scene->clearFocus();

                        node->setSelected(true);

                        emit flow_node_selected((QtFlowNode*)node);

                        _mode = Mode_Move;
                    }

                    break;
                }
                else if (item->type() == QtFlowLink::Type)
                {
                    _scene->clearSelection();
                    _scene->clearFocus();
                    item->setSelected(true);
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
    _scene->update();
    _last_mouse_pos = mouse_event->pos();
}
void QtFlowGraphView::mouseMoveEvent(QMouseEvent* mouse_event)
{
    switch (_mode)
    {
    case Mode_Move:
        for (auto& item : _scene->selectedItems())
        {
            if (item->type() == QtFlowNode::Type)
            {
                QPointF delta = mapToScene(mouse_event->pos()) - mapToScene(_last_mouse_pos);
                ((QtFlowNode*)item)->move_node(item->pos() + delta);
            }
        }
        break;
    case Mode_DragPin:
    {
        if (_highlight_pin)
        {
            _highlight_pin->set_highlight(false);
            _highlight_pin = nullptr;
        }

        QtFlowPin* target_pin = find_pin(mapToScene(mouse_event->pos()));
        if (target_pin)
        {
            // Is target_pin compatible?
            if ((target_pin->pin_type() == FlowPin::In && _temp_link->start()) ||
                (target_pin->pin_type() == FlowPin::Out && _temp_link->end()))
            {
                _highlight_pin = target_pin;
                _highlight_pin->set_highlight(true);
            }
        }

        _temp_link->move_free_end(mapToScene(mouse_event->pos()));
        _temp_link->update();
        QGraphicsView::mouseMoveEvent(mouse_event);
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
    update();

    _last_mouse_pos = mouse_event->pos();
}
void QtFlowGraphView::mouseReleaseEvent(QMouseEvent* mouse_event)
{
    switch (_mode)
    {
    case Mode_DragPin:
    {
        if (!_temp_link)
            break;

        if (_highlight_pin)
        {
            _temp_link->set_pin(_highlight_pin);
            if (_scene->try_add_link(_temp_link))
            {
                _temp_link = nullptr;
            }
            _highlight_pin->set_highlight(false);
            _highlight_pin = nullptr;
        }

        if (_temp_link)
        {
            delete _temp_link;
            _temp_link = nullptr;
        }

        break;
    }
    case Mode_Scroll:
    {
        setDragMode(DragMode::NoDrag);
        QGraphicsView::mouseMoveEvent(mouse_event);
        break;
    }
    default:
        QGraphicsView::mouseMoveEvent(mouse_event);
    };

    _scene->update();
    _last_mouse_pos = mouse_event->pos();

    _mode = Mode_Nothing;
}
void QtFlowGraphView::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Delete)
    {
        for (auto& item : _scene->selectedItems())
        {
            if (item->type() == QtFlowNode::Type)
            {
                QtFlowNode* node = (QtFlowNode*)item;
                _scene->remove_node(node);
                delete node;
            }
            else if (item->type() == QtFlowLink::Type)
            {
                QtFlowLink* link = (QtFlowLink*)item;
                _scene->remove_link(link);
                delete link;
            }
        }
        _scene->clearSelection();
    }

    _scene->update();
    QGraphicsView::keyPressEvent(e);
}
void QtFlowGraphView::wheelEvent(QWheelEvent *e)
{
    qreal zoom = e->delta()*0.0005;
    
    QTransform t = transform();
    t.scale(1 + zoom, 1 + zoom);
    setTransform(t);

    _scene->update();
}

void QtFlowGraphView::drawBackground(QPainter * painter, const QRectF & r)
{
    QGraphicsView::drawBackground(painter, r);

    QBrush brush = backgroundBrush();

    const FlowUIStyle& style = FlowUIStyle::default_style();

    painter->setPen(QPen(style.grid_color_0, 1.0));
    draw_background_grid(painter, 15);

    painter->setPen(QPen(style.grid_color_1, 1.0));
    draw_background_grid(painter, 150);
}
void QtFlowGraphView::draw_background_grid(QPainter* painter, int grid_step)
{
    QPointF tl = mapToScene(rect().topLeft());
    QPointF br = mapToScene(rect().bottomRight());

    int left = std::floor(tl.x() / grid_step - 0.5);
    int right = std::floor(br.x() / grid_step + 1.0);
    int top = std::floor(tl.y() / grid_step - 0.5);
    int bottom = std::floor(br.y() / grid_step + 1.0);

    // vertical 
    for (int x = left; x <= right; ++x)
    {
        QLineF line(x * grid_step, top * grid_step, x * grid_step, bottom * grid_step);
        painter->drawLine(line);
    }

    // horizontal 
    for (int y = top; y <= bottom; ++y)
    {
        QLineF line(left * grid_step, y * grid_step, right * grid_step, y * grid_step);
        painter->drawLine(line);
    }
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
QtFlowPin* QtFlowGraphView::find_pin(const QPointF& scene_pos)
{
    for (auto& item : items(mapFromScene(scene_pos)))
    {
        if (item->type() == QtFlowNode::Type)
        {
            QtFlowNode* node = (QtFlowNode*)item;
            int pin_id = node->check_pin(item->mapFromScene(scene_pos));
            if (pin_id != -1)
                return node->pin(pin_id);
        }
    }
    return nullptr;
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
