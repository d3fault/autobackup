#ifndef USECASEGRAPHICSSCENE_H
#define USECASEGRAPHICSSCENE_H

#include "idesignequalsimplementationgraphicsscene.h"

#include "designequalsimplementationguicommon.h"
#include "../../designequalsimplementationusecase.h"

class DesignEqualsImplementationClass;
class DesignEqualsImplementationClassInstance;
class DesignEqualsImplementationClassLifeLine;
class SignalSlotConnectionActivationArrowForGraphicsScene;
class IRepresentSnapGraphicsItemAndProxyGraphicsItem;
class DesignEqualsImplementationSlotGraphicsItemForUseCaseScene;

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
    IRepresentSnapGraphicsItemAndProxyGraphicsItem *m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone;
    IRepresentSnapGraphicsItemAndProxyGraphicsItem *m_ArrowDestinationSnapper_OrZeroIfNone;

    //Types of interest for various modes
    //ARROW MOUSE PRESS OR RELEASE MODE
    QList<int> m_ListOfItemTypesForArrowPressOrReleaseMode;
    //SNAPPING
    QList<int> m_ListOfItemTypesIWant_SnapSource;
    QList<int> m_ListOfItemTypesIWant_SnapDestination;

    void privateConstructor(DesignEqualsImplementationUseCase *useCase);
    bool keepArrowForThisMouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    bool itemIsWantedType(QGraphicsItem *itemToCheckIfWant, const QList<int> &m_ListOfItemTypesForArrowPressOrReleaseMode);
    QGraphicsItem *giveMeTopMostItemUnderPointThatIwantInArrowMouseMode_OrZeroIfNoneOfInterest(QPointF pointToLookForItemsWeWant);
    QList<QGraphicsItem *> itemsIWantIntersectingRect(QRectF rectWithinWhichToLookForItemsWeWant, const QList<int> &m_ListOfItemTypesForArrowPressOrReleaseMode);
    QGraphicsItem *findNearestPointOnItemBoundingRectFromPoint(const QList<QGraphicsItem*> &itemsToCheck, QPointF pointToFindNearestEdge);
    qreal calculateDistanceFromPointToNearestPointOnBoundingRect(QPointF pointCalculateNearestEdgeTo, QGraphicsItem *item);
    QPointF calculateNearestPointOnBoundingRectToArbitraryPoint(QPointF pointCalculateNearestEdgeTo, QGraphicsItem *item);
    static QRectF mouseSnappingRect(QPointF mousePoint);

    //static DesignEqualsImplementationClassLifeLineUnitOfExecution* targetUnitOfExecutionIfUnitofExecutionIsUnnamed_FirstAfterTargetIfNamedEvenIfYouHaveToCreateIt(DesignEqualsImplementationClassLifeLine *classLifeline);
    virtual bool wantDragDropEvent(QGraphicsSceneDragDropEvent *event);
signals:
    void addActorToUseCaseRequsted(QPointF position);
    void addClassToUseCaseRequested(DesignEqualsImplementationClass *classToAdd, DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline, QPointF position);
    void insertSlotInvocationUseCaseEventRequested(int indexToInsertStatementAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassSlot *slot, SignalEmissionOrSlotInvocationContextVariables slotInvocationContextVariables);
    void insertSignalSlotActivationUseCaseEventRequested(int indexToInsertStatementAt, DesignEqualsImplementationClassSlot *sourceSlotToPutEmitStatementIn_OrZeroIfSourceIsActor, DesignEqualsImplementationClassSignal *signal, DesignEqualsImplementationClassSlot *destinationSlot, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariables, int indexInto_m_ClassLifeLines_OfSignal, int indexInto_m_ClassLifeLines_OfSlot);
    void insertSignalEmissionUseCaseEventRequested(int indexToInsertStatementAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassSignal *signal, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariables);

    void setUseCaseSlotEntryPointRequested(int classLifeLinesIndex, DesignEqualsImplementationClassSlot *useCaseSlotEntryPoint);
    void setExitSignalRequested(DesignEqualsImplementationClassSlot *sourceSlot, DesignEqualsImplementationClassSignal *signal, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariables);

    void e(const QString &msg);
private slots:
    void handleActorAdded(DesignEqualsImplementationActor *actor);
    void handleClassLifeLineAdded(DesignEqualsImplementationClassLifeLine *newClassLifeLine);
    void handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventTypeEnum, QObject* event, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionOrSlotInvocationContextVariables);
    void handleSlotGraphicsItemInsertedIntoClassLifeLineGraphicsItem(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *slotGraphicsItem);
public slots:
    void setMouseMode(DesignEqualsImplementationMouseModeEnum newMouseMode);
};

#endif // USECASEGRAPHICSSCENE_H
