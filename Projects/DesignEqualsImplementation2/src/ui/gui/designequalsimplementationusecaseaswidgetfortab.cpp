#include "designequalsimplementationusecaseaswidgetfortab.h"

#include "../../designequalsimplementation.h"

#include <QMutexLocker>

//TODOreq: if an object lifeline is too short vertically, it can grow (going back to "line" mode so that other stuff can "go through" (en entire vertical line [with "thick" segments whenever an event is received"] represents an object on an event loop, however (OT'ish now:) i still do want some way be able to know that two lifelines share an event-loop/thread)) when a slot is invoked on it. basically the purpose of this req is "auto lifeline height management". combining the best of dia/umbrello (both are shit). arrows should never go through the "thick" portions of a lifeline, since that's just fucking ugly/stupid. right now i'm thinking all use case lines are horizontal (perhaps auxillarySignalX type signals with no connections at design time are the exception, maybe they should be exactly 45 degrees to be easily identified (AND TODOoptional: at the end of that 45 degree signal could be the "use cases that signal triggers", which [double-]clicking on opens up the tab for (that use case list would most likely start off empty, but would be awesome to see as the design progressed))), and only class diagram view can have arbitrarily angled lines (usually denoting inheritence!)
//TODOoptional: the list of classes (user designed/defined (had:de[s|f]igned)) that can be dragged n dropped should have a text filter for easy finding (class count may grow huge in a project)
DesignEqualsImplementationUseCaseAsWidgetForTab::DesignEqualsImplementationUseCaseAsWidgetForTab(DesignEqualsImplementationUseCase *useCase, QWidget *parent)
    : QWidget(parent)
{ }
void DesignEqualsImplementationUseCaseAsWidgetForTab::handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventTypeEnum, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionOrSlotInvocationContextVariables)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    //TODOreq: draw the fucking use case event blah
}
