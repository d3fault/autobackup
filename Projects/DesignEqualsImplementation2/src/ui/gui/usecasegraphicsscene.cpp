#include "usecasegraphicsscene.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>

#include <QMutexLocker>
#include "../../designequalsimplementation.h"

#include "designequalsimplementationguicommon.h"
#include "designequalsimplementationclasslifelinegraphicsitemforusecasescene.h"
#include "../../designequalsimplementationclass.h"
#include "../../designequalsimplementationclasslifeline.h"

UseCaseGraphicsScene::UseCaseGraphicsScene(DesignEqualsImplementationUseCase *useCase)
    : IDesignEqualsImplementationGraphicsScene()
{
    privateConstructor(useCase);
}
UseCaseGraphicsScene::UseCaseGraphicsScene(DesignEqualsImplementationUseCase *useCase, const QRectF &sceneRect)
    : IDesignEqualsImplementationGraphicsScene(sceneRect)
{
    privateConstructor(useCase);
}
UseCaseGraphicsScene::UseCaseGraphicsScene(DesignEqualsImplementationUseCase *useCase, qreal x, qreal y, qreal width, qreal height)
    : IDesignEqualsImplementationGraphicsScene(x, y, width, height)
{
    privateConstructor(useCase);
}
UseCaseGraphicsScene::~UseCaseGraphicsScene()
{ }
void UseCaseGraphicsScene::handleAcceptedDropEvent(QGraphicsSceneDragDropEvent *event)
{
    QByteArray umlItemData = event->mimeData()->data(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_USE_CASE_OBJECT);
    QDataStream dataStream(&umlItemData, QIODevice::ReadOnly);
    bool isActor;
    dataStream >> isActor;
    if(!isActor)
    {
        quintptr classBeingAddedAsQuintPtr;
        dataStream >> classBeingAddedAsQuintPtr;
        DesignEqualsImplementationClass *classBeingAdded = reinterpret_cast<DesignEqualsImplementationClass*>(classBeingAddedAsQuintPtr);
        emit addClassToUseCaseRequested(classBeingAdded, event->scenePos()); //TODOmb: since I'm uglily serializing a pointer, one way to ensure it's valid would be to invokeMethod on classBeingAdded. That would let/allow/make/harness/whatever Qt to do the safety checking for me :-P. Of course it'd ruin my design a tad if I went THROUGH class in order to add class to a use case...
    }
    else
    {
        emit addActorToUseCaseRequsted(event->scenePos()); //TODOoptional: bleh adding an actor is practically pointless [unless it's possible for use case is triggered by a signal (so nvm)], maybe i should add one right when the use case is created (nvmnvm). OK OK: how about if they drag a class onto a use case that has no actor or signal-entry-point, THEN i can auto-add an actor (or ask the user what they want (preemptive error detection))
    }
}
void UseCaseGraphicsScene::privateConstructor(DesignEqualsImplementationUseCase *useCase)
{
    //requests
    connect(this, SIGNAL(addActorToUseCaseRequsted(QPointF)), useCase, SLOT(addActorToUseCase(QPointF)));
    connect(this, SIGNAL(addClassToUseCaseRequested(DesignEqualsImplementationClass*,QPointF)), useCase, SLOT(addClassToUseCase(DesignEqualsImplementationClass*,QPointF)));
    //TODOreq: scene, add use case event requested, use case, addUseCaseEvent

    //responses
    connect(useCase, SIGNAL(actorAdded(QPointF)), this, SLOT(handleActorAdded(QPointF)));
    connect(useCase, SIGNAL(classLifeLineAdded(DesignEqualsImplementationClassLifeLine*)), this, SLOT(handleClassLifeLineAdded(DesignEqualsImplementationClassLifeLine*)));
    connect(useCase, SIGNAL(eventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)), this, SLOT(handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)));
}
bool UseCaseGraphicsScene::wantDragDropEvent(QGraphicsSceneDragDropEvent *event)
{
    return (event->dropAction() == Qt::LinkAction && event->mimeData()->hasFormat(DESIGNEQUALSIMPLEMENTATION_MIME_TYPE_UML_USE_CASE_OBJECT));
}
void UseCaseGraphicsScene::handleActorAdded(QPointF position)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    //TODOreq
    //TODOreq: don't add more than 1 actor (it can be deleted and re-added however)
}
void UseCaseGraphicsScene::handleClassLifeLineAdded(DesignEqualsImplementationClassLifeLine *newClassLifeLine)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);

    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene *designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene = new DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene(newClassLifeLine); //TODOreq: right-click -> change thread (main/gui thread (default), new thread X, existing thread Y). they should somehow visually indicate their thread. i was thinking a shared "filled-background" (color), but even if just the colors of the lifelines are the same that would be sufficient

    designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene->setPos(newClassLifeLine->position()); //TODOreq: listen for moves

    addItem(designEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene);
}
void UseCaseGraphicsScene::handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventTypeEnum, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionOrSlotInvocationContextVariables)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    //TODOreq: draw the fucking use case event blah
}
