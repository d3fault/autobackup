#include "adeditorview.h"

AdEditorView::AdEditorView(Database *dbInstance, WContainerWidget *parent)
    : WContainerWidget(parent),
      m_MyDb(dbInstance)
{
    addWidget(new WText("asdfasdf Ad Editor View"));
    addWidget(new WBreak());
    addWidget(new WText("xD LoL"));
}
