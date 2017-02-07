#include <Core/Common.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Python/PythonFunction.h>
#include <Core/Qt/WindowManager.h>

#include <Flow/FlowModule.h>
#include <Flow/Qt/QtFlowUI.h>

#include "Qt/QtVisNode.h"
#include "VisModule.h"
#include "VisNode.h"
#include "VisPythonModule.h"

PLUTO_IMPLEMENT_MODULE(VisModule);

VisModule* VisModule::s_instance = nullptr;

VisModule::VisModule()
{
    if (s_instance == nullptr)
        s_instance = this;
}
VisModule::~VisModule()
{
    if (s_instance == this)
        s_instance = nullptr;
}

void VisModule::install()
{
    VisPythonModule::create();
}
void VisModule::uninstall()
{
}
void VisModule::init()
{
    FlowModule& flow = FlowModule::instance();

    flow.ui()->install_ui_node_factory<QtVisNode>("vis_node");

    VisNode* tpl = object_new<VisNode>();
    flow.install_node_template(tpl);
    tpl->release();
}
VisModule& VisModule::instance()
{
    return *s_instance;
}

