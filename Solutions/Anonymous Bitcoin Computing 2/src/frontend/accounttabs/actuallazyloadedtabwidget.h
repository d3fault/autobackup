#ifndef ACTUALLAZYLOADEDTABWIDGET_H
#define ACTUALLAZYLOADEDTABWIDGET_H

#include <Wt/WTabWidget>
#include <Wt/WMenuItem>
using namespace Wt;

#include "iaccounttabwidgettabbody.h"

class ActualLazyLoadedTabWidget : public WTabWidget //OLD (was protected but eh): we don't want to expose the regular addTab method, otherwise we might segfault in handleCurrentTabChanged if a non IAccountTabWidgetTabBody tab is added
{
public:
    ActualLazyLoadedTabWidget(WContainerWidget *parent = 0);
    WMenuItem *myAddTab(IAccountTabWidgetTabBody *child, const WString& label,
              LoadPolicy loadingPolicy = LazyLoading);
private:
    void handleCurrentTabChanged();
};

#endif // ACTUALLAZYLOADEDTABWIDGET_H
