#include "mouseormotionormysexyfaceviewmakergui.h"

#include <QCoreApplication>

MouseOrMotionOrMySexyFaceViewMakerGui::MouseOrMotionOrMySexyFaceViewMakerGui(QObject *parent) :
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
    m_UpdateIntervalMs = 100;
    if(arguments.size() > 3)
    {
        int updateInterval = arguments.at(3).toInt(&convertOk);
        if(convertOk && updateInterval > 0)
        {
            m_UpdateIntervalMs = updateInterval;
        }
    }
    m_BottomPixelRowsToIgnore = 25;
    if(arguments.size() > 4)
    {
        int bottomPixelRowsToIgnore = arguments.at(4).toInt(&convertOk);
        if(convertOk && bottomPixelRowsToIgnore > -1) //&& < resolutionHeight
        {
            m_BottomPixelRowsToIgnore = bottomPixelRowsToIgnore;
        }
    }
    m_CameraDeviceName = "/dev/video0";
    if(arguments.size() > 5)
    {
        QString cameraDeviceNameString = arguments.at(5).trimmed();
        if(!cameraDeviceNameString.isEmpty())
            m_CameraDeviceName = cameraDeviceNameString.toUtf8();

    }
    m_CameraResolution.setWidth(720);
    if(arguments.size() > 6)
    {
        int cameraResolutionWidth = arguments.at(6).toInt(&convertOk);
        if(convertOk && cameraResolutionWidth > 0)
        {
            m_CameraResolution.setWidth(cameraResolutionWidth);
        }
    }
    m_CameraResolution.setHeight(480);
    //m_CameraResolution.setHeight(452); //derp damn bug was funnily hard to hunt down xD
    if(arguments.size() > 7)
    {
        int cameraResolutionHeight = arguments.at(7).toInt(&convertOk);
        if(convertOk && cameraResolutionHeight > 0)
        {
            m_CameraResolution.setHeight(cameraResolutionHeight);
        }
    }
    //TODOoptional: could do drawn cursor image path, whether or not to save the image as video, etc...

    m_Gui = new MouseOrMotionOrMySexyFaceViewMakerWidget(m_ViewSize);
}
MouseOrMotionOrMySexyFaceViewMakerGui::~MouseOrMotionOrMySexyFaceViewMakerGui()
{
    if(m_Gui)
        delete m_Gui;
}
void MouseOrMotionOrMySexyFaceViewMakerGui::handleMouseAndOrMotionViewMakerReadyForConnections()
{
    MouseOrMotionOrMySexyFaceViewMaker *business = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(business, SIGNAL(presentPixmapForViewingRequested(QPixmap)), m_Gui, SLOT(presentPixmapForViewing(QPixmap)));

    m_Gui->show();
    QMetaObject::invokeMethod(business, "startMakingMouseOrMotionOrMySexyFaceViews", Qt::QueuedConnection, Q_ARG(QSize, m_ViewSize), Q_ARG(int, m_UpdateIntervalMs), Q_ARG(int, m_BottomPixelRowsToIgnore), Q_ARG(QString, m_CameraDeviceName), Q_ARG(QSize, m_CameraResolution));
}
void MouseOrMotionOrMySexyFaceViewMakerGui::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}
