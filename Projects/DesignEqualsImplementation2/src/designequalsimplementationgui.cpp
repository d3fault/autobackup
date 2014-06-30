#include "designequalsimplementationgui.h"

#include <QCoreApplication>
#include <QStringList>

#include "objectonthreadgroup.h"
#include "designequalsimplementation.h"
#include "designequalsimplementationmainwindow.h"

DesignEqualsImplementationGui::DesignEqualsImplementationGui(QObject *parent)
    : QObject(parent)
    , m_Gui(0)
{
    ObjectOnThreadGroup *objectOnThreadGroup = new ObjectOnThreadGroup(this);
    objectOnThreadGroup->addObjectOnThread<DesignEqualsImplementation>("handleDesignEqualsImplementationReadyForConnections");
    objectOnThreadGroup->doneAddingObjectsOnThreads();
}
DesignEqualsImplementationGui::~DesignEqualsImplementationGui()
{
    if(m_Gui)
        delete m_Gui;
}
void DesignEqualsImplementationGui::handleDesignEqualsImplementationReadyForConnections(QObject *designEqualsImplementationAsQObject)
{
    DesignEqualsImplementation *designEqualsImplementation = static_cast<DesignEqualsImplementation*>(designEqualsImplementationAsQObject);

    QStringList argz = QCoreApplication::arguments();
    if(argz.size() > 1)
    {
        QMetaObject::invokeMethod(designEqualsImplementation, "loadProjectFromFilePath", Q_ARG(QString, argz.at(1)));
    }
    else
    {
        QMetaObject::invokeMethod(designEqualsImplementation, "newEmptyProject");
    }

    m_Gui = new DesignEqualsImplementationMainWindow();
    m_Gui->show();
}
