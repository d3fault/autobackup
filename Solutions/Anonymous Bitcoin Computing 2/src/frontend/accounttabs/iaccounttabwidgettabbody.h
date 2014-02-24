#ifndef IACCOUNTTABWIDGETTABBODY_H
#define IACCOUNTTABWIDGETTABBODY_H

#include <Wt/WContainerWidget>
using namespace Wt;

class AnonymousBitcoinComputingWtGUI;

class IAccountTabWidgetTabBody : public WContainerWidget
{
public:
    IAccountTabWidgetTabBody(AnonymousBitcoinComputingWtGUI *abcApp) : m_PopulatedAndInitializing(false), m_AbcApp(abcApp) { }
    void onTabBeingShown() { if(!m_PopulatedAndInitializing) { populateAndInitialize(); m_PopulatedAndInitializing = true; } }
protected:
    bool m_PopulatedAndInitializing; //h4x perh4ps?
    virtual void populateAndInitialize()=0;
    AnonymousBitcoinComputingWtGUI *m_AbcApp; //shitty hacks/lazy
};

#endif // IACCOUNTTABWIDGETTABBODY_H
