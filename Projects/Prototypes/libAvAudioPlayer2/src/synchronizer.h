#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <QObject>

class Synchronizer : public QObject
{
    Q_OBJECT
public:
    explicit Synchronizer(QObject *parent = 0);

signals:
    void playAudio(QByteArray audioBytes);
    void d(const QString &);
public slots:
    void handleNewAudioDataAvailable(QByteArray newAudio);
    void init();
    void handleNeedAudio(int);
};

#endif // SYNCHRONIZER_H
