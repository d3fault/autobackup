#ifndef BUSINESSOBJECT_H
#define BUSINESSOBJECT_H

#include <QObject>

#include "businessobjectrequestresponsecontracts.h"

class BusinessObject : public QObject
{
    Q_OBJECT
public:
    explicit BusinessObject(QObject *parent = 0);
private:
    BusinessObjectRequestResponseContracts m_RequestResponseContracts;

    int m_X;
signals:
    void someSlotFinished(bool success, bool xIsEven);
    void anotherSlotFinished(bool success);
public slots:
    void someSlot(int x);
    void anotherSlot();
private slots:
    void someSlotContinuation();
};

#endif // BUSINESSOBJECT_H
