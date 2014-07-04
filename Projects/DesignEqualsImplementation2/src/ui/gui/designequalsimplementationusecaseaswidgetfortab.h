#ifndef DESIGNEQUALSIMPLEMENTATIONUSECASEASWIDGETFORTAB_H
#define DESIGNEQUALSIMPLEMENTATIONUSECASEASWIDGETFORTAB_H

#include <QWidget>

#include "ihaveagraphicsviewandscene.h"
#include "../../designequalsimplementationusecase.h"

class DesignEqualsImplementationUseCaseAsWidgetForTab : public IHaveAGraphicsViewAndScene
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationUseCaseAsWidgetForTab(DesignEqualsImplementationUseCase *useCase, QWidget *parent = 0);
public slots:
    void handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventTypeEnum, QObject* event, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionOrSlotInvocationContextVariables);
};

#endif // DESIGNEQUALSIMPLEMENTATIONUSECASEASWIDGETFORTAB_H
