#ifndef BUSINESSOBJECT_H
#define BUSINESSOBJECT_H

#include <QObject>

#include "someslotrequestresponse.h"
#include "someslotfinishedsignalemitterondestruct.h"

class BusinessObject : public QObject
{
    Q_OBJECT
public:
    explicit BusinessObject(QObject *parent = 0);
private:
    SomeSlotRequestResponse m_SomeSlotRequestResponse;
signals:
    void someSlotFinished(bool success);
public slots:
    void someSlot();
private slots:
    void someSlotContinuation();
};

#endif // BUSINESSOBJECT_H
