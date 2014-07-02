#include "designequalsimplementationprojectaswidgetforopenedprojectstabwidget.h"

#include <QTabWidget>

#include "../../designequalsimplementationproject.h"

//This name might seem overly verbose, but I recall vividly in designEquals1 my confusion when naming it "ProjectTabWidget" (the tabwidget, or the widget in the tab!?!?... dun dun dun)
DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget::DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent)
    : QWidget(parent)
{
    QTabWidget *classDiagramAndUseCasesTabWidget = new QTabWidget(this);
    classDiagramAndUseCasesTabWidget->addTab(new DesignEqualsImplementationClassDiagramAsWidgetForTab(classDiagramAndUseCasesTabWidget), tr("Class Diagram"));

    //connect(designEqualsImplementationProject, SIGNAL(classAdded)); //JUST REALIZED/UNDERSTOOD I NEED IMPLICIT SHARING TO BE THREAD SAFE BETTAR MUAHAHAHA, otherwise this classAdded signal here would have to had converted all my *class, *slot, *signal, *args, etc types (and there are LISTS of them to tell the GUI about!) to something the GUI would understand (QStrings, LISTS of QStrings, hahahahaha)
}
