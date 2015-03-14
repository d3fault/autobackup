#include "musicfingersbackendtestergui.h"

#include <QDebug>

#include "objectonthreadgroup.h"
#include "musicfingers.h"
#include "musicfingersbackendtesterwidget.h"

//#define MusicFingersBackendTesterGui_USE_SEPARATE_THREAD

MusicFingersBackendTesterGui::MusicFingersBackendTesterGui(QObject *parent)
    : QObject(parent)
{
#ifdef MusicFingersBackendTesterGui_USE_SEPARATE_THREAD
    qRegisterMetaType<Finger::FingerEnum>("Finger::FingerEnum");
    ObjectOnThreadGroup *backendThread = new ObjectOnThreadGroup(this);
    backendThread->addObjectOnThread<MusicFingers>("handleMusicFingersReadyForConnections");
    backendThread->doneAddingObjectsOnThreads();
#else
    MusicFingers *musicFingers = new MusicFingers(this);
    handleMusicFingersReadyForConnections(musicFingers);
#endif
}
void MusicFingersBackendTesterGui::handleMusicFingersReadyForConnections(QObject *musicFingersAsQObject)
{
    MusicFingers *business = static_cast<MusicFingers*>(musicFingersAsQObject);
    connect(business, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    MusicFingersBackendTesterWidget *gui = new MusicFingersBackendTesterWidget();
    connect(gui, SIGNAL(fingerMoved(Finger::FingerEnum,int)), business, SLOT(moveFinger(Finger::FingerEnum,int)));
    gui->show();
}
void MusicFingersBackendTesterGui::handleO(const QString &msg)
{
    qWarning() << msg;
}
