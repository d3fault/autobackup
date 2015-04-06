#ifndef MUSICFINGERSSYNTHESISTOOLKIT_H
#define MUSICFINGERSSYNTHESISTOOLKIT_H

#include <QObject>
#include <QTextStream>

#include "finger.h"

class MusicFingersSynthesisToolkit : public QObject
{
    Q_OBJECT
public:
    explicit MusicFingersSynthesisToolkit(QObject *parent = 0);
private:
    QTextStream m_StdOut;
    QTextStream m_StdErr;
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
