#include "actuallazyloadedtabwidget.h"

//oh the joys of polymorphism :)
ActualLazyLoadedTabWidget::ActualLazyLoadedTabWidget(WContainerWidget *parent)
    : WTabWidget(parent)
{
    currentChanged().connect(this, &ActualLazyLoadedTabWidget::handleCurrentTabChanged);
}
WMenuItem *ActualLazyLoadedTabWidget::addTab(IAccountTabWidgetTabBody *child, const WString &label, WTabWidget::LoadPolicy loadingPolicy)
{
    bool firstAdd = count() == 0 ? true : false;
    WMenuItem *ret = addTab(child, label, loadingPolicy);
    if(firstAdd)
    {
        child->onTabBeingShown();
    }
    return ret;
}
void ActualLazyLoadedTabWidget::handleCurrentTabChanged()
{
    static_cast<IAccountTabWidgetTabBody*>(currentWidget())->onTabBeingShown();
}
