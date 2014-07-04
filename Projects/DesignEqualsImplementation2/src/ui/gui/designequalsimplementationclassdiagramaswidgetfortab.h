#ifndef DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H
#define DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H

#include <QWidget>

#include "../../designequalsimplementationcommon.h"

class DesignEqualsImplementationProject;
class DesignEqualsImplementationClass;
class ClassDiagramGraphicsScene;

class DesignEqualsImplementationClassDiagramAsWidgetForTab : public QWidget
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationClassDiagramAsWidgetForTab(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent = 0);
private:
    ClassDiagramGraphicsScene *m_ClassDiagramScene;
public slots:
    void handleClassAdded(DesignEqualsImplementationClass *classAdded);
};

#endif // DESIGNEQUALSIMPLEMENTATIONCLASSDIAGRAMASWIDGETFORTAB_H
