#include "libavplayerwidget.h"
#ifndef QT_NO_OPENGL
    #include <QtOpenGL/QGLWidget>
#endif

#include "../backend/libavplayerbackend.h"
#include "libavgraphicsitem.h"
#include "rawaudioplayer.h"

libAvPlayerWidget::libAvPlayerWidget(QWidget *parent)
    : QWidget(parent)
{
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection); //schedule the init, for now we just want to get the widget shown asap
}
libAvPlayerWidget::~libAvPlayerWidget()
{

}
void libAvPlayerWidget::init()
{
    initGui();
    initBackend();
}
void libAvPlayerWidget::initGui()
{
    m_Layout = new QVBoxLayout();
    m_GraphicsScene = new QGraphicsScene(this);
    m_GraphicsView = new QGraphicsView(m_GraphicsScene);
    m_LibAvGraphicsItem = new LibAvGraphicsItem();
#ifndef QT_NO_OPENGL
    m_GraphicsView->setViewport(new QGLWidget);
#endif
    m_GraphicsScene->addItem(m_LibAvGraphicsItem);
    m_Layout->addWidget(m_GraphicsView);

    m_ControlsLayout = new QHBoxLayout();
    m_PlayButton = new QPushButton();
    m_PlayButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
    connect(m_PlayButton, SIGNAL(clicked()), m_LibAvGraphicsItem, SLOT(startLibAVThreads()));
    m_ControlsLayout->addWidget(m_PlayButton);
    m_Layout->addLayout(m_ControlsLayout);

    this->setLayout(m_Layout);

    m_RawAudioPlayer = new RawAudioPlayer();
    m_RawAudioPlayer->start();
}
void libAvPlayerWidget::initBackend()
{
    m_BackendThread = new QThread();
    m_PlayerBackend = new LibAvPlayerBackend();
    m_PlayerBackend->moveToThread(m_BackendThread);
    m_BackendThread->start();

    //connections
    connect(m_PlayerBackend, SIGNAL(frameReadyToBePresented(const QVideoFrame &)), m_LibAvGraphicsItem, SLOT(presentFrame(const QVideoFrame &)));
    connect(m_PlayerBackend, SIGNAL(audioReadyToBePresented(const QByteArray &)), m_RawAudioPlayer, SLOT(playAudio(const QByteArray &))); //TODO: not sure if the audio player should be on the gui thread or if it can/should be on a separate thread

    //start player backend
    QMetaObject::invokeMethod(m_PlayerBackend, "start", Qt::QueuedConnection);
}
