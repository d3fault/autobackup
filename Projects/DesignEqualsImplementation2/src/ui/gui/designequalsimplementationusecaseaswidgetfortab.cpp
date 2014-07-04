#include "designequalsimplementationusecaseaswidgetfortab.h"

#include <QMutexLocker>
#include "../../designequalsimplementation.h"

#include "usecasegraphicsscene.h"
#include "designequalsimplementationclasslifelinegraphicsitemforusecasescene.h"

//TODOreq: if an object lifeline is too short vertically, it can grow (going back to "line" mode so that other stuff can "go through" (en entire vertical line [with "thick" segments whenever an event is received"] represents an object on an event loop, however (OT'ish now:) i still do want some way be able to know that two lifelines share an event-loop/thread)) when a slot is invoked on it. basically the purpose of this req is "auto lifeline height management". combining the best of dia/umbrello (both are shit). arrows should never go through the "thick" portions of a lifeline, since that's just fucking ugly/stupid. right now i'm thinking all use case lines are horizontal (perhaps auxillarySignalX type signals with no connections at design time are the exception, maybe they should be exactly 45 degrees to be easily identified (AND TODOoptional: at the end of that 45 degree signal could be the "use cases that signal triggers", which [double-]clicking on opens up the tab for (that use case list would most likely start off empty, but would be awesome to see as the design progressed))), and only class diagram view can have arbitrarily angled lines (usually denoting inheritence!)
//TODOoptional: the list of classes (user designed/defined (had:de[s|f]igned)) that can be dragged n dropped should have a text filter for easy finding (class count may grow huge in a project)
DesignEqualsImplementationUseCaseAsWidgetForTab::DesignEqualsImplementationUseCaseAsWidgetForTab(DesignEqualsImplementationUseCase *useCase, QWidget *parent)
    : IHaveAGraphicsViewAndScene(new UseCaseGraphicsScene(), parent)
{
    //requests
    connect(static_cast<UseCaseGraphicsScene*>(m_GraphicsScene), SIGNAL(addActorToUseCaseRequsted(QPointF)), useCase, SLOT(addActorToUseCase(QPointF)));
    connect(static_cast<UseCaseGraphicsScene*>(m_GraphicsScene), SIGNAL(addClassToUseCaseRequested(DesignEqualsImplementationClass*,QPointF)), useCase, SLOT(addClassToUseCase(DesignEqualsImplementationClass*,QPointF)));
    //TODOreq: scene, add use case event requested, use case, addUseCaseEvent

    //responses
    connect(useCase, SIGNAL(actorAdded(QPointF)), this, SLOT(handleActorAdded(QPointF)));
    connect(useCase, SIGNAL(classAdded(DesignEqualsImplementationClass*,QPointF)), this, SLOT(handleClassAdded(DesignEqualsImplementationClass*,QPointF)));
    connect(useCase, SIGNAL(eventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)), this, SLOT(handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)));
}
void DesignEqualsImplementationUseCaseAsWidgetForTab::handleActorAdded(QPointF position)
{
    //TODOreq
    //TODOreq: don't add more than 1 actor (it can be deleted and re-added however)
}
void DesignEqualsImplementationUseCaseAsWidgetForTab::handleClassAdded(DesignEqualsImplementationClass *newClass, QPointF position)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);

    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene = new DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(newClass); //TODOreq: right-click -> change thread (main/gui thread (default), new thread X, existing thread Y). they should somehow visually indicate their thread. i was thinking a shared "filled-background" (color), but even if just the colors of the lifelines are the same that would be sufficient

    designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene->setPos(position); //TODOreq: listen for moves
    //TODOreq: for classes it makes sense to store the position in the class (as long as we remember it's the position in the class diagram), but i need to store the position somewhere else for the class's occurance in use case

    m_GraphicsScene->addItem(designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene);
}
void DesignEqualsImplementationUseCaseAsWidgetForTab::handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventTypeEnum, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionOrSlotInvocationContextVariables)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    //TODOreq: draw the fucking use case event blah
}
