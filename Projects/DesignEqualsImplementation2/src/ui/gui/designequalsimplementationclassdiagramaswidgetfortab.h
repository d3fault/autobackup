#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H

#include <QWidget>

#include "ihaveagraphicsviewandscene.h"
#include "classdiagramgraphicsscene.h"
#include "../../designequalsimplementationcommon.h"

class DesignEqualsImplementationProject;
class DesignEqualsImplementationClass;

class DesignEqualsImplementationClassDiagramAsWidgetForTab : public IHaveAGraphicsViewAndScene
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassDiagramAsWidgetForTab(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent = 0);
public slots:
    void handleClassAdded(DesignEqualsImplementationClass *classAdded);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H
