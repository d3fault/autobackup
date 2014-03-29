#include "mouseandormotionviewmakergui.h"

#include <QCoreApplication>

MouseAndOrMotionViewMakerGui::MouseAndOrMotionViewMakerGui(QObject *parent) :
    QObject(parent), m_Gui(0)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleMouseAndOrMotionViewMakerReadyForConnections()));
    m_BusinessThread.start();

    //optional arguments parsing
    QStringList arguments = QCoreApplication::arguments();
    bool convertOk;
    m_ViewSize.setWidth(800);
    if(arguments.size() > 1)
    {
        int viewWidth = arguments.at(1).toInt(&convertOk);
        if(convertOk && viewWidth > 0)
        {
            m_ViewSize.setWidth(viewWidth);
        }
    }
    m_ViewSize.setHeight(600);
    if(arguments.size() > 2)
    {
        int viewHeight = arguments.at(2).toInt(&convertOk);
        if(convertOk && viewHeight > 0)
        {
            m_ViewSize.setHeight(viewHeight);
        }
    }
    m_UpdateIntervalMs = 200;
    if(arguments.size() > 3)
    {
        int updateInterval = arguments.at(3).toInt(&convertOk);
        if(convertOk && updateInterval > 0)
        {
            m_UpdateIntervalMs = updateInterval;
        }
    }
    //TODOoptional: could do drawn cursor image path, whether or not to save the image as video, etc...

    m_Gui = new MouseAndOrMotionViewMakerWidget(m_ViewSize);
}
MouseAndOrMotionViewMakerGui::~MouseAndOrMotionViewMakerGui()
{
    if(m_Gui)
        delete m_Gui;
}
void MouseAndOrMotionViewMakerGui::handleMouseAndOrMotionViewMakerReadyForConnections()
{
    MouseAndOrMotionViewMaker *business = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(m_Gui, SIGNAL(startMakingMouseAndOrMotionViewsRequested()), business, SLOT(startMakingMouseAndOrMotionViews()));
    connect(business, SIGNAL(presentPixmapForViewingRequested(QPixmap)), m_Gui, SLOT(presentPixmapForViewing(QPixmap)));

    m_Gui->show();
    QMetaObject::invokeMethod(business, "startMakingMouseAndOrMotionViews", Qt::QueuedConnection, Q_ARG(QSize, m_ViewSize), Q_ARG(int, m_UpdateIntervalMs));
}
void MouseAndOrMotionViewMakerGui::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}
