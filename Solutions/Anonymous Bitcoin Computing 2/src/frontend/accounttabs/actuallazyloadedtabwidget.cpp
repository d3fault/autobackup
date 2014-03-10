#include "actuallazyloadedtabwidget.h"

//oh the joys of polymorphism :)
ActualLazyLoadedTabWidget::ActualLazyLoadedTabWidget(WContainerWidget *parent)
    : WTabWidget(parent)
{ }
WMenuItem *ActualLazyLoadedTabWidget::myAddTab(IAccountTabWidgetTabBody *child, const WString &label, WTabWidget::LoadPolicy loadingPolicy)
{
    bool firstAdd = count() == 0 ? true : false;
    WMenuItem *ret = WTabWidget::addTab(child, label, loadingPolicy);
    if(firstAdd)
    {
        child->onTabBeingShown();
    }
    else
    {
        //ret->triggered().connect(child, &IAccountTabWidgetTabBody::onTabBeingShown); //TODOoptional: either disconnect after first invocation, or also connect to the firstAdd'd one and use it to like 'refresh' or whatever. It doesn't really matter if we don't disconnect it though, because inside onTabBeingShown is a bool protector anyways...
    }
    return ret;
}
