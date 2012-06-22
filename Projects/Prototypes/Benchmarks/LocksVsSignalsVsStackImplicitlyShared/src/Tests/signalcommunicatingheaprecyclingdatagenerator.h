#ifndef SIGNALCOMMUNICATINGHEAPRECYCLINGDATAGENERATOR_H
#define SIGNALCOMMUNICATINGHEAPRECYCLINGDATAGENERATOR_H

#include <QObject>
#include <QList>
#include <QByteArray>

class SignalCommunicatingHeapRecyclingDataGenerator : public QObject
{
    Q_OBJECT
public:
    explicit SignalCommunicatingHeapRecyclingDataGenerator(int loopCount, int sizeMultiplier, const QString &dataToBeMultiplied);
    void cleanup();
private:
    QList<QString*> *m_RecycleList;
    int m_Interval;
    int m_NumGenerated;
    void generateOne();
    QString *getRecycledOrNew();
    QString m_StartingVal;
    QString m_ReplaceTo;
    bool m_Done;
    void checkDone();

    int m_LoopCount;
    int m_SizeMultiplier;
signals:
    void d(const QString &);
    void testFinished(int numGenerated, int totalSize);
    void bytesGenerated(QString *bytes);
public slots:
    void init();
    void startTest();
    void recycledUsed(QString *used);
};

#endif // SIGNALCOMMUNICATINGHEAPRECYCLINGDATAGENERATOR_H
