#ifndef LIBAVAUDIOPLAYERWIDGET_H
#define LIBAVAUDIOPLAYERWIDGET_H

#include <QtGui/QWidget>
#include <QThread>

class ThreadSafeQueueByMutex;
class libAvAudioPlayer;
class QtAudioPlayer;

class libAvAudioPlayerWidget : public QWidget
{
    Q_OBJECT

public:
    libAvAudioPlayerWidget(QWidget *parent = 0);
    ~libAvAudioPlayerWidget();
private:
    ThreadSafeQueueByMutex *m_Queue;
    QThread *m_DecodeThread;
    libAvAudioPlayer *m_Decoder;
    QThread *m_AudioThread;
    QtAudioPlayer *m_Player;
private slots:
    void init();
};

#endif // LIBAVAUDIOPLAYERWIDGET_H
