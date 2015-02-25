#include "musicfingersbackendtestergui.h"

#include "objectonthreadgroup.h"
#include "musicfingers.h"
#include "musicfingersbackendtesterwidget.h"

MusicFingersBackendTesterGui::MusicFingersBackendTesterGui(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<Finger::FingerEnum>("Finger::FingerEnum");

    ObjectOnThreadGroup *backendThread = new ObjectOnThreadGroup(this);
    backendThread->addObjectOnThread<MusicFingers>("handleMusicFingersReadyForConnections");
    backendThread->doneAddingObjectsOnThreads();
}
void MusicFingersBackendTesterGui::handleMusicFingersReadyForConnections(QObject *musicFingersAsQObject)
{
    MusicFingers *business = static_cast<MusicFingers*>(musicFingersAsQObject);
    MusicFingersBackendTesterWidget *gui = new MusicFingersBackendTesterWidget();
    connect(gui, SIGNAL(fingerMoved(Finger::FingerEnum,int)), business, SLOT(moveFinger(Finger::FingerEnum,int)));
    gui->show();
}
