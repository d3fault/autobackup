#include "libavaudioplayer2test.h"

libAvAudioPlayer2test::libAvAudioPlayer2test(QWidget *parent)
    : QWidget(parent)
{
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}
void libAvAudioPlayer2test::init()
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_PlayButton = new QPushButton("Play");
    m_Layout->addWidget(m_PlayButton);
    m_Layout->addWidget(m_Debug);
    this->setLayout(m_Layout);

    m_Thread = new QThread();
    m_Player = new libAvAudioPlayer2();
    m_Player->moveToThread(m_Thread);
    m_Thread->start();

    connect(m_PlayButton, SIGNAL(clicked()), m_Player, SLOT(play()));
    connect(m_Player, SIGNAL(d(const QString &)), this, SLOT(handleD(const QString &)));

    QMetaObject::invokeMethod(m_Player, "init", Qt::QueuedConnection);
}
void libAvAudioPlayer2test::handleD(const QString &text)
{
    m_Debug->appendPlainText(text);
}
libAvAudioPlayer2test::~libAvAudioPlayer2test()
{
}
