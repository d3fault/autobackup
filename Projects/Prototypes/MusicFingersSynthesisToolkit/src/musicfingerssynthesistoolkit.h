#ifndef MUSICFINGERSSYNTHESISTOOLKIT_H
#define MUSICFINGERSSYNTHESISTOOLKIT_H

#include <QObject>
#include <QTextStream>
#include <QHash>

#include "finger.h"

class MusicFingersSynthesisToolkit : public QObject
{
    Q_OBJECT
public:
    explicit MusicFingersSynthesisToolkit(QObject *parent = 0);
private:
    QHash<Finger::FingerEnum, qint64 /*plucked at msec since epoch*/> m_FingersPluckedAt_OrZeroIfNotPlucked;
    QTextStream m_StdOut;
    QTextStream m_StdErr;

    int channelAkaGroupToPluckFrequency(int channelAkaGroup);
    void switchChannelAkaVoiceGroupToInstrumentAndTurnNoteOn(int channelAkaVoiceGroup, int instrumentIndex);
signals:
    void exitRequested(int exitCode);
public slots:
    void startSynthesizingToStkToolkitStdinFromMusicFingersSerialPort();
private slots:
    void handleO(const QString &msg);
    void handleE(const QString &msg);
    void handleFingerMoved(Finger::FingerEnum finger, int position);
};

#endif // MUSICFINGERSSYNTHESISTOOLKIT_H
