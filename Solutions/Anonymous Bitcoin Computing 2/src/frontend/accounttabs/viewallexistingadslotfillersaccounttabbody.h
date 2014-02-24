#ifndef VIEWALLEXISTINGADSLOTFILLERSACCOUNTTABBODY_H
#define VIEWALLEXISTINGADSLOTFILLERSACCOUNTTABBODY_H

#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WBreak>

#include "iaccounttabwidgettabbody.h"

using namespace Wt;

class ViewAllExistingAdSlotFillersAccountTabBody : public IAccountTabWidgetTabBody
{
public:
    ViewAllExistingAdSlotFillersAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp);
private:
    void populateAndInitialize();
};

#endif // VIEWALLEXISTINGADSLOTFILLERSACCOUNTTABBODY_H
