#include <Core/Common.h>

#include <Core/Image/Image.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Qt/WindowManager.h>

#include <Flow/FlowContext.h>
#include <Flow/FlowGraph.h>
#include <Flow/FlowPin.h>
#include <Flow/FlowProperty.h>

#include "Qt/QtVisNode.h"
#include "Qt/QtVisWindow.h"
#include "VisNode.h"

PYTHON_OBJECT_IMPL(VisNode, "VisNode")
{
    cls;
}

VisNode::VisNode()
{
    //_current_window = nullptr;

    //set_attribute("node_class", "vis.VisNode");
    //set_attribute("title", "Show");
    //set_attribute("category", "Image");
    //set_attribute("doc", "");

    //set_attribute("ui_class", "vis_node");

    //add_pin("In", FlowPin::In);

    //add_property(object_new<BoolProperty>("window", false));
}
VisNode::~VisNode()
{
}
void VisNode::run(FlowContext* ctx)
{
    if (!is_pin_linked("In"))
        return;

    if (_ui_node)
    {
        Image img = ctx->read_pin_<Image>("In");
        if (!img.valid())
            return;

        bool use_window = attribute<bool>("window");

        if (use_window)
        {
            if (!_current_window)
            {
                _current_window = PlutoCore::instance().window_manager()->create_window<QtVisWindow>();
            }
            QMetaObject::invokeMethod(_current_window, "show", Qt::BlockingQueuedConnection);
            QMetaObject::invokeMethod(_current_window, "set_image", Qt::BlockingQueuedConnection, Q_ARG(const Image&, img));
        }
        else if (_ui_node)
        {
            QMetaObject::invokeMethod(_ui_node, "show_image", Qt::BlockingQueuedConnection, Q_ARG(const Image&, img));
        }
    }
}
