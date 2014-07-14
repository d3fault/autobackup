#ifndef USECASEGRAPHICSSCENE_H
#define USECASEGRAPHICSSCENE_H

#include "idesignequalsimplementationgraphicsscene.h"

#include "designequalsimplementationguicommon.h"
#include "../../designequalsimplementationusecase.h"

class DesignEqualsImplementationClass;
class HasA_Private_Classes_Members_ListEntryType;
class DesignEqualsImplementationClassLifeLine;
class SignalSlotConnectionActivationArrowForGraphicsScene;
class SnappingIndicationVisualRepresentation;

class UseCaseGraphicsScene : public IDesignEqualsImplementationGraphicsScene
{
    Q_OBJECT
public:
    UseCaseGraphicsScene(DesignEqualsImplementationUseCase *useCase);
    UseCaseGraphicsScene(DesignEqualsImplementationUseCase *useCase, const QRectF &sceneRect);
    UseCaseGraphicsScene(DesignEqualsImplementationUseCase *useCase, qreal x, qreal y, qreal width, qreal height);
    DesignEqualsImplementationMouseModeEnum mouseMode() const;
    virtual ~UseCaseGraphicsScene();
protected:
    virtual void handleAcceptedDropEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
    DesignEqualsImplementationUseCase *m_UseCase;
    DesignEqualsImplementationMouseModeEnum m_MouseMode;
    SignalSlotConnectionActivationArrowForGraphicsScene *m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn;
    SnappingIndicationVisualRepresentation *m_ItemThatSnappingForCurrentMousePosWillClick_OrZeroIfNone;

    void privateConstructor(DesignEqualsImplementationUseCase *useCase);
    bool keepArrowForThisMouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    bool wantItemInArrowMouseMode(QGraphicsItem *itemToCheckIfWant);
    QGraphicsItem *giveMeTopMostItemUnderPointThatIwantInArrowMouseMode_OrZeroIfNoneOfInterest(QPointF pointToLookForItemsWeWant);
    QList<QGraphicsItem *> itemsIWantIntersectingRect(QRectF rectWithinWhichToLookForItemsWeWant);
    QGraphicsItem *findNearestPointOnItemBoundingRectFromPoint(const QList<QGraphicsItem*> &itemsToCheck, QPointF pointToFindNearestEdge);
    qreal calculateDistanceFromPointToNearestPointOnBoundingRect(QPointF pointCalculateNearestEdgeTo, QGraphicsItem *item);
    QPointF calculateNearestPointOnBoundingRectToArbitraryPoint(QPointF pointCalculateNearestEdgeTo, QGraphicsItem *item);

    //static DesignEqualsImplementationClassLifeLineUnitOfExecution* targetUnitOfExecutionIfUnitofExecutionIsUnnamed_FirstAfterTargetIfNamedEvenIfYouHaveToCreateIt(DesignEqualsImplementationClassLifeLine *classLifeline);
    virtual bool wantDragDropEvent(QGraphicsSceneDragDropEvent *event);
signals:
    void addActorToUseCaseRequsted(QPointF position);
    void addClassToUseCaseRequested(DesignEqualsImplementationClass *classToAdd, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position);
    void insertSlotInvocationUseCaseEventRequested(int indexToInsertStatementAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLineUnitOfExecution *destinationUnitOfExecution_OrZeroIfDestIsActor, DesignEqualsImplementationClassSlot *slot,SignalEmissionOrSlotInvocationContextVariables slotInvocationContextVariables);
    void insertSignalSlotActivationUseCaseEventRequested(int indexToInsertStatementAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLineUnitOfExecution *destinationUnitOfExecution_OrZeroIfDestIsActor, DesignEqualsImplementationClassSignal *signal, DesignEqualsImplementationClassSlot *slot, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariables);
    void insertSignalEmissionUseCaseEventRequested(int indexToInsertStatementAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLineUnitOfExecution *destinationUnitOfExecution_OrZeroIfDestIsActor, DesignEqualsImplementationClassSignal *signal, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariables);
    void setExitSignalRequested(DesignEqualsImplementationClassLifeLineUnitOfExecution *sourceUnitOfExecution, DesignEqualsImplementationClassSignal *signal, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariables);

    void e(const QString &msg);
private slots:
    void handleActorAdded(DesignEqualsImplementationActor *actor);
    void handleClassLifeLineAdded(DesignEqualsImplementationClassLifeLine *newClassLifeLine);
    void handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventTypeEnum, QObject* event, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionOrSlotInvocationContextVariables);
public slots:
    void setMouseMode(DesignEqualsImplementationMouseModeEnum newMouseMode);
};

#endif // USECASEGRAPHICSSCENE_H
