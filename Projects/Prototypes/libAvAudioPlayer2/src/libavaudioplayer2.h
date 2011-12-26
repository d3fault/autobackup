#ifndef LIBAVAUDIOPLAYER2_H
#define LIBAVAUDIOPLAYER2_H

#include <QObject>
#include <QThread>

#include <curldownloader.h>
#include <libavaudiodecoder.h>
#include <synchronizer.h>
#include <qtaudioplayer.h>

class libAvAudioPlayer2 : public QObject
{
    Q_OBJECT
public:
    explicit libAvAudioPlayer2(QObject *parent = 0);
private:
    QThread *m_LoaderThread;
    QThread *m_DecoderThread;
    QThread *m_SynchronizerThread;
    QThread *m_AudioThread;

    ILoadMediaStreamsIntoMemory *m_Loader;
    libAvAudioDecoder *m_Decoder;
    Synchronizer *m_Synchronizer;
    QtAudioPlayer *m_AudioPlayer;
signals:
    void d(const QString &);
public slots:
    void init();
    void play();
};

#endif // LIBAVAUDIOPLAYER2_H
