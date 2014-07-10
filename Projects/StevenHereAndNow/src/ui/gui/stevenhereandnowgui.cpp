#include "stevenhereandnowgui.h"

#include "objectonthreadgroup.h"
#include "../../stevenhereandnow.h"
#include "stevenhereandnowwidget.h"

StevenHereAndNowGui::StevenHereAndNowGui(QObject *parent)
    : QObject(parent)
{
    ObjectOnThreadGroup *stevenHereAndNowThread = new ObjectOnThreadGroup(this);
    stevenHereAndNowThread->addObjectOnThread<StevenHereAndNow>("handleStevenHereAndNowReadyForConnections");
    stevenHereAndNowThread->doneAddingObjectsOnThreads();
    m_GuiPtr.reset(new StevenHereAndNowWidget());
}
void StevenHereAndNowGui::handleStevenHereAndNowReadyForConnections(QObject *stevenHereAndNowAsQObject)
{
    StevenHereAndNow *stevenHereAndNow = static_cast<StevenHereAndNow*>(stevenHereAndNowAsQObject);

    connect(m_GuiPtr.data(), SIGNAL(sayRequested(QString)), stevenHereAndNow, SLOT(handleSayRequested(QString))/* life would be much simpler like this: SLOT(say(QString))*/);
    //zzzz (so that's what happens)
}
