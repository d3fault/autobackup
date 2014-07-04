#include "designequalsimplementationprojectaswidgetforopenedprojectstabwidget.h"

#include <QVBoxLayout>
#include <QTabWidget>
#include <QMutexLocker>
#include <QGraphicsScene>

#include "../../designequalsimplementation.h"
#include "../../designequalsimplementationproject.h"
#include "designequalsimplementationclassdiagramaswidgetfortab.h"
#include "designequalsimplementationusecaseaswidgetfortab.h"
#include "designequalsimplementationguicommon.h"

//This name might seem overly verbose, but I recall vividly in designEquals1 my confusion when naming it "ProjectTabWidget" (the tabwidget, or the widget in the tab!?!?... dun dun dun)... and now to add to the confusioin, it "isA" TabWidget as well as a widget used for a tab!!! rofl
DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget::DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget(DesignEqualsImplementationProject *project, QWidget *parent)
    : QWidget(parent)
    , m_ClassDiagramAndUseCasesTabWidget(new QTabWidget())
    , m_DesignEqualsImplementationProject(project)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    myLayout->setContentsMargins(DESIGNEQUALSIMPLEMENTATION_GUI_LAYOUT_CONTENT_MARGINS, DESIGNEQUALSIMPLEMENTATION_GUI_LAYOUT_CONTENT_MARGINS, DESIGNEQUALSIMPLEMENTATION_GUI_LAYOUT_CONTENT_MARGINS, DESIGNEQUALSIMPLEMENTATION_GUI_LAYOUT_CONTENT_MARGINS);

    DesignEqualsImplementationClassDiagramAsWidgetForTab *designEqualsImplementationClassDiagramAsWidgetForTab = new DesignEqualsImplementationClassDiagramAsWidgetForTab(project);
    m_ClassDiagramAndUseCasesTabWidget->addTab(designEqualsImplementationClassDiagramAsWidgetForTab, tr("Class Diagram")); //TODOreq: class diagram tab uncloseable

    //connect(designEqualsImplementationProject, SIGNAL(classAdded)); //JUST REALIZED/UNDERSTOOD I NEED IMPLICIT SHARING TO BE THREAD SAFE BETTAR MUAHAHAHA, otherwise this classAdded signal here would have to had converted all my *class, *slot, *signal, *args, etc types (and there are LISTS of them to tell the GUI about!) to something the GUI would understand (QStrings, LISTS of QStrings, hahahahaha)
    //^Meh, even though I _COULD_ solve it using implicit sharing, I'm wondering if a mutex is easier/better (whichever has less code is better imo (BUT mutex might lock the GUI so... it goes both ways)). Idfk just studying implicit sharing and thinking about frontend<-->backend in general [for this specific app]

    connect(this, SIGNAL(newUseCaseRequested()), project, SLOT(handleNewUseCaseRequested()));
    connect(project, SIGNAL(useCaseAdded(DesignEqualsImplementationUseCase*)), this, SLOT(handleUseCaseAdded(DesignEqualsImplementationUseCase*)));

    //After beginning coding with a mutex (lazy :-P) for synchronization, I'm realizing that I think I need to have tons of "classAdded", "slotAdded", etc signals ANYWAYS. Well I mean idfk tbh, it depends on if I want to use reactor pattern or just do the backend "adding" as well as front-end "adding" in the front-end code. Even what I was considering doing with implicit sharing does NOT use reactor pattern... so... idfk... maybe I can't save myself from typing all that shit out no matter what I do. I'm pretty damn sure reactor pattern is better and more future proof and eases synchronization in the long run anyways... so...
    //^After all my tons of "classAdded" etc signals are connected to corresponding slots, we have to tell the backend to tell us about them via:

    //TODOreq: listening for "classAdded" type signals is good reactor pattern design, but iterating lists directly is fine with implicit sharing [or locking xD]. since when everything is "up and running" it's definitely better to listen to "classAdded" type signals, it makes the list iteration code redundant (unless i emit the lists themselves?). the invokeMethod on the next line was being used, but commenting out now because it caused a deadlock heh. NVM it wasn't the source! it's because addTab signal calls it's slots directly and doesn't let a mutex go out of scope. weird, the solution to that (hackily) is to make it a QueuedConnection xD. death to mutexes
    QMetaObject::invokeMethod(project, "emitAllClassesAndUseCasesInProject"); //When a class (both pointer and "Name" is received via signal, the same thing is done for the Signals/Slots/Properties etc... so on and so forth until the Class Diagram is populated)

    //Actually I'm wrong, I _CAN_ (should) use reactor pattern with implicit sharing, but for now I'm going to emulate it hackily with mutex (lazy)

    myLayout->addWidget(m_ClassDiagramAndUseCasesTabWidget, 1);
    setLayout(myLayout);
}
QTabWidget *DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget::classDiagramAndUseCasesTabWidget()
{
    return m_ClassDiagramAndUseCasesTabWidget;
}
DesignEqualsImplementationProject *DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget::designEqualsImplementationProject()
{
    return m_DesignEqualsImplementationProject;
}
void DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget::requestNewUseCase()
{
    emit newUseCaseRequested();
}
void DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget::handleUseCaseAdded(DesignEqualsImplementationUseCase *useCase) //TODOreq: use case OPENED instead? or i guess right here could just add it to our list of use cases
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    DesignEqualsImplementationUseCaseAsWidgetForTab *designEqualsImplementationUseCaseAsWidgetForTab = new DesignEqualsImplementationUseCaseAsWidgetForTab(useCase);
    int tabIndex = m_ClassDiagramAndUseCasesTabWidget->addTab(designEqualsImplementationUseCaseAsWidgetForTab, useCase->Name);
    m_ClassDiagramAndUseCasesTabWidget->setCurrentIndex(tabIndex);
}
