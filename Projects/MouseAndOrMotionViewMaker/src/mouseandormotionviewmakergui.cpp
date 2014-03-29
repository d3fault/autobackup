#include "mouseandormotionviewmakergui.h"

#include <QCoreApplication>

MouseAndOrMotionViewMakerGui::MouseAndOrMotionViewMakerGui(QObject *parent) :
    QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleMouseAndOrMotionViewMakerReadyForConnections()));
    m_BusinessThread.start();
}
void MouseAndOrMotionViewMakerGui::handleMouseAndOrMotionViewMakerReadyForConnections()
{
    MouseAndOrMotionViewMaker *business = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(&m_Gui, SIGNAL(startMakingMouseAndOrMotionViewsRequested()), business, SLOT(startMakingMouseAndOrMotionViews()));
    connect(business, SIGNAL(presentPixmapForViewingRequested(QPixmap)), &m_Gui, SLOT(presentPixmapForViewing(QPixmap)));

    m_Gui.show();
    QMetaObject::invokeMethod(business, "startMakingMouseAndOrMotionViews", Qt::QueuedConnection);
}
void MouseAndOrMotionViewMakerGui::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}
