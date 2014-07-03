#include "designequalsimplementationprojectaswidgetforopenedprojectstabwidget.h"

#include <QVBoxLayout>
#include <QTabWidget>
#include <QMutexLocker>

#include "../../designequalsimplementation.h"
#include "../../designequalsimplementationproject.h"
#include "designequalsimplementationclassdiagramaswidgetfortab.h"
#include "designequalsimplementationusecaseaswidgetfortab.h"

//This name might seem overly verbose, but I recall vividly in designEquals1 my confusion when naming it "ProjectTabWidget" (the tabwidget, or the widget in the tab!?!?... dun dun dun)... and now to add to the confusioin, it "isA" TabWidget as well as a widget used for a tab!!! rofl
DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget::DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget(DesignEqualsImplementationProject *designEqualsImplementationProject, QWidget *parent)
    : QWidget(parent)
    , m_ClassDiagramAndUseCasesTabWidget(new QTabWidget())
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    DesignEqualsImplementationClassDiagramAsWidgetForTab *designEqualsImplementationClassDiagramAsWidgetForTab = new DesignEqualsImplementationClassDiagramAsWidgetForTab(designEqualsImplementationProject);
    m_ClassDiagramAndUseCasesTabWidget->addTab(designEqualsImplementationClassDiagramAsWidgetForTab, tr("Class Diagram")); //TODOreq: class diagram tab uncloseable

    //connect(designEqualsImplementationProject, SIGNAL(classAdded)); //JUST REALIZED/UNDERSTOOD I NEED IMPLICIT SHARING TO BE THREAD SAFE BETTAR MUAHAHAHA, otherwise this classAdded signal here would have to had converted all my *class, *slot, *signal, *args, etc types (and there are LISTS of them to tell the GUI about!) to something the GUI would understand (QStrings, LISTS of QStrings, hahahahaha)
    //^Meh, even though I _COULD_ solve it using implicit sharing, I'm wondering if a mutex is easier/better (whichever has less code is better imo (BUT mutex might lock the GUI so... it goes both ways)). Idfk just studying implicit sharing and thinking about frontend<-->backend in general [for this specific app]

    connect(designEqualsImplementationProject, SIGNAL(classAdded(DesignEqualsImplementationClass*)), designEqualsImplementationClassDiagramAsWidgetForTab, SLOT(handleClassAdded(DesignEqualsImplementationClass*))); //TODOreq: deleting a part of a class in class diagram might completely invalidate a use case, so either that should trigger the entire deletion of said use case (after warning), or force them into going and fixing the use case, or similar
    connect(designEqualsImplementationProject, SIGNAL(useCaseAdded(DesignEqualsImplementationUseCase*)), this, SLOT(handleUseCaseAdded(DesignEqualsImplementationUseCase*)));

    //After beginning coding with a mutex (lazy :-P) for synchronization, I'm realizing that I think I need to have tons of "classAdded", "slotAdded", etc signals ANYWAYS. Well I mean idfk tbh, it depends on if I want to use reactor pattern or just do the backend "adding" as well as front-end "adding" in the front-end code. Even what I was considering doing with implicit sharing does NOT use reactor pattern... so... idfk... maybe I can't save myself from typing all that shit out no matter what I do. I'm pretty damn sure reactor pattern is better and more future proof and eases synchronization in the long run anyways... so...
    //^After all my tons of "classAdded" etc signals are connected to corresponding slots, we have to tell the backend to tell us about them via:
    QMetaObject::invokeMethod(designEqualsImplementationProject, "emitAllClassesAndUseCasesInProject"); //When a class (both pointer and "Name" is received via signal, the same thing is done for the Signals/Slots/Properties etc... so on and so forth until the Class Diagram is populated)

    //Actually I'm wrong, I _CAN_ (should) use reactor pattern with implicit sharing, but for now I'm going to emulate it hackily with mutex (lazy)

    myLayout->addWidget(m_ClassDiagramAndUseCasesTabWidget, 1);
    setLayout(myLayout);
}
void DesignEqualsImplementationProjectAsWidgetForOpenedProjectsTabWidget::handleUseCaseAdded(DesignEqualsImplementationUseCase *useCase) //TODOreq: use case OPENED instead? or i guess right here could just add it to our list of use cases
{
    QMutexLocker scopedLock(&DesignEqualsImplementation::BackendMutex);
    DesignEqualsImplementationUseCaseAsWidgetForTab *designEqualsImplementationUseCaseAsWidgetForTab = new DesignEqualsImplementationUseCaseAsWidgetForTab(useCase);
    m_ClassDiagramAndUseCasesTabWidget->addTab(designEqualsImplementationUseCaseAsWidgetForTab, useCase->Name);

    connect(useCase, SIGNAL(eventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)), designEqualsImplementationUseCaseAsWidgetForTab, SLOT(handleEventAdded(DesignEqualsImplementationUseCase::UseCaseEventTypeEnum,QObject*,SignalEmissionOrSlotInvocationContextVariables)));
}
