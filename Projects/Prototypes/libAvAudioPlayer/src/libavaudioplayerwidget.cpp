#include "libavaudioplayerwidget.h"

#include <threadsafequeuebymutex.h>
#include <libavaudioplayer.h>
#include <qtaudioplayer.h>

libAvAudioPlayerWidget::libAvAudioPlayerWidget(QWidget *parent)
    : QWidget(parent)
{
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}
libAvAudioPlayerWidget::~libAvAudioPlayerWidget()
{

}
void libAvAudioPlayerWidget::init()
{
    m_Queue = new ThreadSafeQueueByMutex();


    m_DecodeThread = new QThread();
    m_Decoder = new libAvAudioPlayer(m_Queue);
    m_Decoder->moveToThread(m_DecodeThread);
    m_DecodeThread->start();

    m_AudioThread = new QThread();
    m_Player = new QtAudioPlayer(m_Queue);
    m_Player->moveToThread(m_AudioThread);
    m_AudioThread->start();


    connect(m_Decoder, SIGNAL(audioSpecGathered(int,int,int)), m_Player, SLOT(setAudioSpec(int,int,int)));


    QMetaObject::invokeMethod(m_Decoder, "initAndPlay", Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_Player, "start", Qt::QueuedConnection);
}
