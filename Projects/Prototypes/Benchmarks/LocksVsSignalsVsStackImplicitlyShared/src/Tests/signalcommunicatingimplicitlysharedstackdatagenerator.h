#ifndef SIGNALCOMMUNICATINGIMPLICITLYSHAREDSTACKDATAGENERATOR_H
#define SIGNALCOMMUNICATINGIMPLICITLYSHAREDSTACKDATAGENERATOR_H

#include <QObject>

class SignalCommunicatingImplicitlySharedStackDataGenerator : public QObject
{
    Q_OBJECT
public:
    explicit SignalCommunicatingImplicitlySharedStackDataGenerator(int loopCount, int sizeMultiplier, const QString &dataToBeMultiplied);
private:
    void generateOne();
    int m_Interval;
    QString m_StartingVal;
    QString m_ReplaceTo;
    int m_LoopCount;
    int m_SizeMultiplier;
signals:
    //void testFinished();
    void bytesGenerated(QString);
public slots:
    void init();
    void startTest();
};

#endif // SIGNALCOMMUNICATINGIMPLICITLYSHAREDSTACKDATAGENERATOR_H
