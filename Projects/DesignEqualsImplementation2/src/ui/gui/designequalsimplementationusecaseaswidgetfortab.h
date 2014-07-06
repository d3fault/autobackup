#ifndef DESIGNEQUALSIMPLEMENTATIONUSECASEASWIDGETFORTAB_H
#define DESIGNEQUALSIMPLEMENTATIONUSECASEASWIDGETFORTAB_H

#include "ihaveagraphicsviewandscene.h"

#include "designequalsimplementationguicommon.h"

class DesignEqualsImplementationUseCase;

class DesignEqualsImplementationUseCaseAsWidgetForTab : public IHaveAGraphicsViewAndScene
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationUseCaseAsWidgetForTab(DesignEqualsImplementationUseCase *useCase, QWidget *parent = 0);
signals:
    void mouseModeChanged(DesignEqualsImplementationMouseModeEnum newMouseMode);
};

#endif // DESIGNEQUALSIMPLEMENTATIONUSECASEASWIDGETFORTAB_H
