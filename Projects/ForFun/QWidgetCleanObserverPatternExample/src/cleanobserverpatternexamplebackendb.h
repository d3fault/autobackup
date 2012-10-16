#ifndef CLEANOBSERVERPATTERNEXAMPLEBACKENDB_H
#define CLEANOBSERVERPATTERNEXAMPLEBACKENDB_H

#include <QObject>
#include <QTimer>

class CleanObserverPatternExampleBackendB : public QObject
{
    Q_OBJECT
public:
    explicit CleanObserverPatternExampleBackendB(QObject *parent = 0);
private:
    QTimer *m_Timer;
    int m_MaxNumberToGenerate;
    int m_NumberToWatchFor;
signals:
    void randomNumberResultsGathered(const QString &stringResult);
public slots:
    void threadStartingUp();
    void updateRandomNumberGeneratorProperties(int maxNum, int numberToWatchFor);
    void handleTimerTimedOut();
    void threadShuttingDown();
};

#endif // CLEANOBSERVERPATTERNEXAMPLEBACKENDB_H
