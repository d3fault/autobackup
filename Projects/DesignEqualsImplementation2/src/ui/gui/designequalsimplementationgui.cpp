#include "designequalsimplementationgui.h"

#include <QCoreApplication>
#include <QStringList>

#include "objectonthreadgroup.h"
#include "designequalsimplementation.h"
#include "designequalsimplementationmainwindow.h"

DesignEqualsImplementationGui::DesignEqualsImplementationGui(QObject *parent)
    : QObject(parent)
{
    ObjectOnThreadGroup *objectOnThreadGroup = new ObjectOnThreadGroup(this);
    objectOnThreadGroup->addObjectOnThread<DesignEqualsImplementation>("handleDesignEqualsImplementationReadyForConnections");
    objectOnThreadGroup->doneAddingObjectsOnThreads();
    m_Gui = new DesignEqualsImplementationMainWindow();
}
DesignEqualsImplementationGui::~DesignEqualsImplementationGui()
{
    delete m_Gui;
}
void DesignEqualsImplementationGui::handleDesignEqualsImplementationReadyForConnections(QObject *designEqualsImplementationAsQObject)
{
    DesignEqualsImplementation *designEqualsImplementation = static_cast<DesignEqualsImplementation*>(designEqualsImplementationAsQObject);

    m_Gui->showMaximized(); //TODOreq: show() keeps hiding my title bar above the screen, no clue why

    connect(m_Gui, SIGNAL(newProjectRequested()), designEqualsImplementation, SLOT(newProject()));
    connect(m_Gui, SIGNAL(openExistingProjectRequested(QString)), designEqualsImplementation, SLOT(openExistingProject(QString)));
    connect(designEqualsImplementation, SIGNAL(projectOpened(DesignEqualsImplementationProject*)), m_Gui, SLOT(handleProjectOpened(DesignEqualsImplementationProject*)));

    QStringList argz = QCoreApplication::arguments();
    if(argz.size() > 1)
    {
        QMetaObject::invokeMethod(designEqualsImplementation, "openExistingProject", Q_ARG(QString, argz.at(1)));
    }
    else
    {
        QMetaObject::invokeMethod(designEqualsImplementation, "newProject");
    }
}
