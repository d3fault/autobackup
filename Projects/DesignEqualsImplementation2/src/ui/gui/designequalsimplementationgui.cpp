#include "designequalsimplementationgui.h"

#include <QCoreApplication>
#include <QStringList>

#include "designequalsimplementation.h"
#include "designequalsimplementationmainwindow.h"

DesignEqualsImplementationGui::DesignEqualsImplementationGui(QObject *parent)
    : QObject(parent)
{
    m_Gui = new DesignEqualsImplementationMainWindow();

    DesignEqualsImplementation *designEqualsImplementation = new DesignEqualsImplementation(this);
    handleDesignEqualsImplementationReadyForConnections(designEqualsImplementation); //threading ended up making it dangerous until COW/etc is implemented. i got lazy with the mutexes and forgot a bunch of places i think. the bulk of the app doesn't need to be multi-threaded anyways. i could maybe justify the source code generation step... but even then only for really large projects (isn't qfile async already so nvm? idk)
}
DesignEqualsImplementationGui::~DesignEqualsImplementationGui()
{
    delete m_Gui;
}
void DesignEqualsImplementationGui::handleDesignEqualsImplementationReadyForConnections(QObject *designEqualsImplementationAsQObject)
{
    DesignEqualsImplementation *designEqualsImplementation = static_cast<DesignEqualsImplementation*>(designEqualsImplementationAsQObject);

    m_Gui->showMaximized(); //TODOreq: show() works fine now, no clue why, but keeping maximized for testing

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
