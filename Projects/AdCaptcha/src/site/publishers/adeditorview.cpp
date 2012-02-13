#include "adeditorview.h"

AdEditorView::AdEditorView(Database *dbInstance, WContainerWidget *parent)
    : WContainerWidget(parent),
      m_MyDb(dbInstance)
{
    addWidget(new WText("asdfasdf Ad Editor View"));
    addWidget(new WBreak());
    addWidget(new WText("xD LoL"));
    addWidget(new WBreak());
    WAnchor *ownerAnchor = new WAnchor(AdCaptchaSite::getOwnerPath(), "Owner Page");
    ownerAnchor->setRefInternalPath(AdCaptchaSite::getOwnerPath());
    addWidget(ownerAnchor);
}
