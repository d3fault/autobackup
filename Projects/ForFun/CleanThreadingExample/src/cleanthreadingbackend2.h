#ifndef CLEANTHREADINGBACKEND2_H
#define CLEANTHREADINGBACKEND2_H

#include <QObject>
#include <QTimer>

class CleanThreadingBackend2 : public QObject
{
    Q_OBJECT
public:
    explicit CleanThreadingBackend2(QObject *parent = 0);
private:
    QTimer m_Timer;
    int m_MaxNumberToGenerate;
    int m_NumberToWatchFor;
signals:
    void randomNumberResultsGathered(const QString &stringResult);
public slots:
    void updateRandomNumberGeneratorProperties(int maxNum, int numberToWatchFor);
    void handleTimerTimedOut();
};

#endif // CLEANTHREADINGBACKEND2_H
