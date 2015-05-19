#include "icepickgui.h"

#include "objectonthreadgroup.h"
#include "../lib/icepick.h"
#include "icepickwidget.h"

IcePickGui::IcePickGui(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<YearRange>("YearRange");
    qRegisterMetaType<IceEntry>("IceEntry");

    ObjectOnThreadGroup *objectOnThreadGroup = new ObjectOnThreadGroup(this);
    objectOnThreadGroup->addObjectOnThread<IcePick>("handleIcePickReadyForConnections");
    objectOnThreadGroup->doneAddingObjectsOnThreads();

    m_Gui = new IcePickWidget();
}
IcePickGui::~IcePickGui()
{
    delete m_Gui;
}
void IcePickGui::handleIcePickReadyForConnections(QObject *icePickAsQObject)
{
    IcePick *icePick = static_cast<IcePick*>(icePickAsQObject);
    connect(m_Gui, SIGNAL(pickIceRequested(YearRange,bool)), icePick, SLOT(pickIce(YearRange,bool)));
    connect(icePick, SIGNAL(e(QString)), m_Gui, SLOT(handleMsg(QString)));
    connect(icePick, SIGNAL(v(QString)), m_Gui, SLOT(handleMsg(QString)));
    connect(icePick, SIGNAL(finishedPickingIce(bool,IceEntry)), m_Gui, SLOT(handleFinishedPickingIce(bool,IceEntry)));
    m_Gui->show();
}

