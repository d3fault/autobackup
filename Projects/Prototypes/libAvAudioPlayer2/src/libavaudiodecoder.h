#ifndef LIBAVAUDIODECODER_H
#define LIBAVAUDIODECODER_H

#include <QObject>

class libAvAudioDecoder : public QObject
{
    Q_OBJECT
public:
    explicit libAvAudioDecoder(QObject *parent = 0);

signals:
    void onSpecGathered(int sampleRate, int numChannels, int sampleSize);
public slots:
    void handleNewDataAvailable(QByteArray newData);
};

#endif // LIBAVAUDIODECODER_H
