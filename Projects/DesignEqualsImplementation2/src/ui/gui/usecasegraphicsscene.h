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
class DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene;

class UseCaseGraphicsScene : public IDesignEqualsImplementationGraphicsScene
{
    Q_OBJECT
public:
    static QGraphicsItem *createVisualRepresentationBasedOnStatementType(IDesignEqualsImplementationStatement *theStatement, int indexInsertedInto, QGraphicsItem *parent);

    UseCaseGraphicsScene(DesignEqualsImplementationUseCase *useCase);
    UseCaseGraphicsScene(DesignEqualsImplementationUseCase *useCase, const QRectF &sceneRect);
    UseCaseGraphicsScene(DesignEqualsImplementationUseCase *useCase, qreal x, qreal y, qreal width, qreal height);
    DesignEqualsImplementationMouseModeEnum mouseMode() const;
    virtual ~UseCaseGraphicsScene();

    DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene* classLifelineGraphicsItemByClassLifeline_OrZeroIfNotFound(DesignEqualsImplementationClassLifeLine *classLifelineToRetrieveGraphicsItemFor);
    DesignEqualsImplementationUseCase *useCase() const;
protected:
    virtual void handleAcceptedDropEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
    DesignEqualsImplementationUseCase *m_UseCase;
    DesignEqualsImplementationMouseModeEnum m_MouseMode;
    SignalSlotConnectionActivationArrowForGraphicsScene *m_SignalSlotConnectionActivationArrowCurrentlyBeingDrawn_OrZeroIfNone;
    IRepresentSnapGraphicsItemAndProxyGraphicsItem *m_ItemThatSourceSnappingForCurrentMousePosWillClick_OrZeroIfNone;
    IRepresentSnapGraphicsItemAndProxyGraphicsItem *m_ArrowDestinationSnapper_OrZeroIfNone;

    //Types of interest for various modes
    //ARROW MOUSE PRESS OR RELEASE MODE
    QList<int> m_ListOfItemTypesForArrowPressOrReleaseMode;
    //SNAPPING
    QList<int> m_ListOfItemTypesIWant_SnapSource;
    QList<int> m_ListOfItemTypesIWant_SnapDestination;

    QList<DesignEqualsImplementationClassLifeLineGraphicsItemForUseCaseScene*> m_ClassLifelineGraphicsItemsInUseCaseGraphicsScene;

    void privateConstructor(DesignEqualsImplementationUseCase *useCase);
    bool processMouseReleaseEvent_andReturnWhetherOrNotToKeepTempArrowForThisMouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    bool itemIsWantedType(QGraphicsItem *itemToCheckIfWant, const QList<int> &listOfTypesTheyWant);
    QGraphicsItem *giveMeTopMostItemUnderPointThatIwantInArrowMouseMode_OrZeroIfNoneOfInterest(QPointF pointToLookForItemsWeWant);
    QList<QGraphicsItem *> itemsIWantIntersectingRect(QRectF rectWithinWhichToLookForItemsWeWant, const QList<int> &listOfTypesTheyWant);
    QGraphicsItem *findNearestPointOnItemBoundingRectFromPoint(const QList<QGraphicsItem*> &itemsToCheck, QPointF pointToFindNearestEdge);
    qreal calculateDistanceFromPointToNearestPointOnBoundingRect(QPointF pointCalculateNearestEdgeTo, QGraphicsItem *item);
    QPointF calculateNearestPointOnBoundingRectToArbitraryPoint(QPointF pointCalculateNearestEdgeTo, QGraphicsItem *item);
    static QRectF mouseSnappingRect(QPointF mousePoint);
    //QPointF calculatePointOnSlotOnClassLifelineThatWeUseAsAStartPoint_Aka_P1_ifWeWereALine_UsingTheIndexThatTheStatementWasInsertedInto(int indexInto_m_ClassLifeLines_OfSignal, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceSlot, DesignEqualsImplementationClassSignal *signalUseCaseEvent, int indexStatementWasInsertedInto);

    //static DesignEqualsImplementationClassLifeLineUnitOfExecution* targetUnitOfExecutionIfUnitofExecutionIsUnnamed_FirstAfterTargetIfNamedEvenIfYouHaveToCreateIt(DesignEqualsImplementationClassLifeLine *classLifeline);
    virtual bool wantDragDropEvent(QGraphicsSceneDragDropEvent *event);
signals:
    void addActorToUseCaseRequsted(QPointF position);
    void addClassToUseCaseRequested(DesignEqualsImplementationClass *classToAdd, /*TODOinstancing: DesignEqualsImplementationClassInstance *myInstanceInClassThatHasMe_OrZeroIfUseCasesRootClassLifeline, */QPointF position);
    void insertSlotInvocationUseCaseEventRequested(int indexToInsertStatementAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassLifeLine *destinationClassLifeline, DesignEqualsImplementationClassSlot *slot, SignalEmissionOrSlotInvocationContextVariables slotInvocationContextVariables);
    void insertSignalSlotActivationUseCaseEventRequested(int indexToInsertStatementAt, DesignEqualsImplementationClassSlot *sourceSlotToPutEmitStatementIn_OrZeroIfSourceIsActor, DesignEqualsImplementationClassSignal *signal, DesignEqualsImplementationClassSlot *destinationSlot, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariables, DesignEqualsImplementationClassLifeLine *indexInto_m_ClassLifeLines_OfSignal, int indexInto_m_ClassLifeLines_OfSlot);
    void insertSignalEmissionUseCaseEventRequested(int indexToInsertStatementAt, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassSignal *signal, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariables, DesignEqualsImplementationClassLifeLine *classLifelineOfSignal);
    void insertExistingSignalNewSlotEventRequested(int indexTheSlotWantsToBeInSignalSlotConnectStatements, DesignEqualsImplementationClassSlot *sourceOrderedListOfStatements_OrZeroIfSourceIsActor, DesignEqualsImplementationClassSignal *designEqualsImplementationClassSignal, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionContextVariables, DesignEqualsImplementationClassLifeLine *indexInto_m_ClassLifeLines_OfSignal, int indexInto_m_ClassLifeLines_OfSlot);

    void setUseCaseSlotEntryPointRequested(DesignEqualsImplementationClassLifeLine *rootClassLifeline, DesignEqualsImplementationClassSlot *useCaseSlotEntryPoint);
    void setExitSignalRequested(DesignEqualsImplementationClassSlot *sourceSlot, DesignEqualsImplementationClassSignal *signal, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariables);

    void e(const QString &msg);
private slots:
    void handleActorAdded(DesignEqualsImplementationActor *actor);
    void handleClassLifeLineAdded(DesignEqualsImplementationClassLifeLine *newClassLifeLine);
    void handleSignalEmitEventAdded(int indexInto_m_ClassLifeLines_OfSignal, IDesignEqualsImplementationHaveOrderedListOfStatements *sourceSlot, DesignEqualsImplementationClassSignal *signalUseCaseEvent, int indexStatementWasInsertedInto);
    void handleSlotAddedToExistingSignalSlotConnectionList(DesignEqualsImplementationClassSignal *existingSignalSlotWasAddedTo, DesignEqualsImplementationClassSlot *slotAdded, int indexOfSignalConnectionsTheSlotWasInsertedInto);
    //void handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventTypeEnum, QObject* event, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionOrSlotInvocationContextVariables);
    void handleSlotGraphicsItemInsertedIntoClassLifeLineGraphicsItem(DesignEqualsImplementationSlotGraphicsItemForUseCaseScene *slotGraphicsItem);
public slots:
    void setMouseMode(DesignEqualsImplementationMouseModeEnum newMouseMode);
};

#endif // USECASEGRAPHICSSCENE_H
