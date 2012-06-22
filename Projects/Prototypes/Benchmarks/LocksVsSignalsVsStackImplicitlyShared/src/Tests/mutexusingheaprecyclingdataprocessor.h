#ifndef MUTEXUSINGHEAPRECYCLINGDATAPROCESSOR_H
#define MUTEXUSINGHEAPRECYCLINGDATAPROCESSOR_H

#include <QObject>

#include "mutexusingheaprecyclingdatagenerator.h"

class MutexUsingHeapRecyclingDataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit MutexUsingHeapRecyclingDataProcessor(MutexUsingHeapRecyclingDataGenerator *listOwner);
private:
    QString m_ReplaceBackTo;
    MutexUsingHeapRecyclingDataGenerator *m_ListOwner;
signals:
    void doneWithData(QString *usedData);
public slots:
    void init();
    void processGeneratedData(QString *generatedData);
};

#endif // MUTEXUSINGHEAPRECYCLINGDATAPROCESSOR_H
