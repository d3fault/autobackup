#ifndef SIGNALCOMMUNICATINGIMPLICITLYSHAREDSTACKDATAPROCESSOR_H
#define SIGNALCOMMUNICATINGIMPLICITLYSHAREDSTACKDATAPROCESSOR_H

#include <QObject>

class SignalCommunicatingImplicitlySharedStackDataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit SignalCommunicatingImplicitlySharedStackDataProcessor(int loopCount);
private:
    QString m_ReplaceBackTo;
    int m_Interval; //we have to count in order to know when done. hell, we'll be the one signaling too :-/
    int m_LoopCount;

    int m_Count;
signals:
    void testFinished();
public slots:
    void init();
    void processGeneratedData(const QString &);
};

#endif // SIGNALCOMMUNICATINGIMPLICITLYSHAREDSTACKDATAPROCESSOR_H
