#ifndef USECASEGRAPHICSSCENE_H
#define USECASEGRAPHICSSCENE_H

#include "idesignequalsimplementationgraphicsscene.h"

#include "designequalsimplementationguicommon.h"
#include "../../designequalsimplementationusecase.h"

class DesignEqualsImplementationClass;
class HasA_Private_Classes_Members_ListEntryType;
class DesignEqualsImplementationClassLifeLine;
class SignalSlotConnectionActivationArrowForGraphicsScene;

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

    void privateConstructor(DesignEqualsImplementationUseCase *useCase);
    bool keepArrowForThisMouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    QGraphicsItem *thereIsAtLeastOneItemLeftInMyListOfItemsUnderTheMouseReleasePointThatIwantInArrowMouseMode(QPointF pointToLookForItemsWeWant);
    void processOneItemUnderMouse();
    virtual bool wantDragDropEvent(QGraphicsSceneDragDropEvent *event);
signals:
    void addActorToUseCaseRequsted(QPointF position);
    void addClassToUseCaseRequested(DesignEqualsImplementationClass *classToAdd, HasA_Private_Classes_Members_ListEntryType *myInstanceInClassThatHasMe_OrZeroIfTopLevelObject, QPointF position);
    void addSlotInvocationUseCaseEventRequested(DesignEqualsImplementationClassSlot*slot,SignalEmissionOrSlotInvocationContextVariables slotInvocationContextVariables);
    void addSignalSlotActivationUseCaseEventRequested(DesignEqualsImplementationClassSignal *signal, DesignEqualsImplementationClassSlot *slot, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariables);
    void addSignalEmissionUseCaseEventRequested(DesignEqualsImplementationClassSignal *signal, SignalEmissionOrSlotInvocationContextVariables signalEmissionContextVariables);

    void e(const QString &msg);
private slots:
    void handleActorAdded(DesignEqualsImplementationActor *actor);
    void handleClassLifeLineAdded(DesignEqualsImplementationClassLifeLine *newClassLifeLine);
    void handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventTypeEnum, QObject* event, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionOrSlotInvocationContextVariables);
public slots:
    void setMouseMode(DesignEqualsImplementationMouseModeEnum newMouseMode);
};

#endif // USECASEGRAPHICSSCENE_H
