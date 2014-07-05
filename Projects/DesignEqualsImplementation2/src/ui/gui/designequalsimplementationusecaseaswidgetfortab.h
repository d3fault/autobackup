#ifndef DESIGNEQUALSIMPLEMENTATIONUSECASEASWIDGETFORTAB_H
#define DESIGNEQUALSIMPLEMENTATIONUSECASEASWIDGETFORTAB_H

#include <QWidget>

#include "ihaveagraphicsviewandscene.h"

class DesignEqualsImplementationUseCase;

class DesignEqualsImplementationUseCaseAsWidgetForTab : public IHaveAGraphicsViewAndScene
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationUseCaseAsWidgetForTab(DesignEqualsImplementationUseCase *useCase, QWidget *parent = 0);
};

#endif // DESIGNEQUALSIMPLEMENTATIONUSECASEASWIDGETFORTAB_H
