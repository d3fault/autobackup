#ifndef MUTEXUSINGHEAPRECYCLINGDATAGENERATOR_H
#define MUTEXUSINGHEAPRECYCLINGDATAGENERATOR_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>

class MutexUsingHeapRecyclingDataGenerator : public QObject
{
    Q_OBJECT
public:
    explicit MutexUsingHeapRecyclingDataGenerator(int loopCount, int dataMultipler, const QString &dataSample);
    void recycledUsed(QString *used);
    void cleanup();
private:
    QList<QString*> *m_RecycleList;
    int m_Interval;
    int m_NumGenerated;
    void generateOne();
    QString *getRecycledOrNew();
    QString m_StartingVal;
    QString m_ReplaceTo;
    QMutex m_RecycleLock;
    bool m_Done;
    void checkDone();

    int m_LoopCount;
    int m_SizeMultiplier;

    int m_Count;
    QString m_CountChar;
signals:
    void testFinished(int numGenerated, int totalSize);
    void bytesGenerated(QString *bytes);
public slots:
    void init();
    void startTest();
};

#endif // MUTEXUSINGHEAPRECYCLINGDATAGENERATOR_H
