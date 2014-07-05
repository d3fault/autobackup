#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H

#include <QWidget>

#include "ihaveagraphicsviewandscene.h"

class DesignEqualsImplementationProject;

class DesignEqualsImplementationClassDiagramAsWidgetForTab : public IHaveAGraphicsViewAndScene
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassDiagramAsWidgetForTab(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent = 0);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H
