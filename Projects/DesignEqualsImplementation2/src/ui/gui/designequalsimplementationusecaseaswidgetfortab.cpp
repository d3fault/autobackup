#include "designequalsimplementationusecaseaswidgetfortab.h"

#include "../../designequalsimplementation.h"

#include <QMutexLocker>

DesignEqualsImplementationUseCaseAsWidgetForTab::DesignEqualsImplementationUseCaseAsWidgetForTab(DesignEqualsImplementationUseCase *useCase, QWidget *parent)
    : QWidget(parent)
{ }
void DesignEqualsImplementationUseCaseAsWidgetForTab::handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum useCaseEventTypeEnum, QObject *event, const SignalEmissionOrSlotInvocationContextVariables &signalEmissionOrSlotInvocationContextVariables)
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    //TODOreq: draw the fucking use case event blah
}
