#ifndef SIGNALCOMMUNICATINGHEAPRECYCLINGDATAPROCESSOR_H
#define SIGNALCOMMUNICATINGHEAPRECYCLINGDATAPROCESSOR_H

#include <QObject>
#include <QByteArray>

class SignalCommunicatingHeapRecyclingDataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit SignalCommunicatingHeapRecyclingDataProcessor(QObject *parent = 0);
private:
    QString m_ReplaceBackTo;
    int m_Count;
signals:
    void doneWithData(QString *usedData);
public slots:
    void init();
    void processGeneratedData(QString *generatedData);
};

#endif // SIGNALCOMMUNICATINGHEAPRECYCLINGDATAPROCESSOR_H
