#include "lifeshaperstaticfilestest.h"

LifeShaperStaticFilesTest::LifeShaperStaticFilesTest(QObject *parent) :
    QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));

    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleLifeShaperStaticFilesBusinessInstantiated()));
    m_BusinessThread.start();
}
void LifeShaperStaticFilesTest::handleLifeShaperStaticFilesBusinessInstantiated()
{
    LifeShaperStaticFilesBusiness *business = m_BusinessThread.getObjectPointerForConnectionsOnly();

    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));

    connect(&m_Gui, SIGNAL(startIteratingEasyTreeHashFileRequested(QString,QString,QString)), business, SLOT(startIteratingEasyTreeHashFile(QString,QString,QString)));
    connect(&m_Gui, SIGNAL(stopIteratingRequested()), business, SLOT(stopIteratingEasyTreeHashFile()));

    connect(business, SIGNAL(nowProcessingEasyTreeHashItem(QString,QString,bool,qint64,QString,QString)), &m_Gui, SLOT(handleNowProcessingEasyTreeHashItem(QString,QString,bool,qint64,QString,QString)));

    connect(&m_Gui, SIGNAL(recursionFromCurrentChanged(bool)), business, SLOT(setRecursivelyApplyFromHereUntilParent(bool)));
    connect(business, SIGNAL(stoppedRecursingDatOperationBecauseParentDirEntered()), &m_Gui, SLOT(handleStoppedRecursingDatOperationBecauseParentDirEntered()));

    connect(&m_Gui, SIGNAL(leaveBehindDecided()), business, SLOT(leaveBehind()));
    connect(&m_Gui, SIGNAL(iffyCopyrightDecided()), business, SLOT(iffyCopyright()));
    connect(&m_Gui, SIGNAL(deferDecisionDecided()), business, SLOT(deferDecision()));
    connect(&m_Gui, SIGNAL(bringForwardDecided()), business, SLOT(bringForward()));
    connect(&m_Gui, SIGNAL(useTHISasReplacementDecided(QString)), business, SLOT(useTHISasReplacement(QString)));

    connect(business, SIGNAL(finishedProcessingEasyTreeHashFile()), &m_Gui, SLOT(handleFinishedProcessingEasyTreeHashFile()));

    m_Gui.show();
}
void LifeShaperStaticFilesTest::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}
