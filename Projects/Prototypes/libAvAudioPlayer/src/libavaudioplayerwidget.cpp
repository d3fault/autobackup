#include "libavaudioplayerwidget.h"

#include <libavaudioplayer.h>

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
    m_Thread = new QThread();
    m_Player = new libAvAudioPlayer();
    m_Player->moveToThread(m_Thread);
    m_Thread->start();

    QMetaObject::invokeMethod(m_Player, "initAndPlay", Qt::QueuedConnection);
}
