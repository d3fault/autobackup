#ifndef BUSINESSOBJECT2_H
#define BUSINESSOBJECT2_H

#include <QObject>

class BusinessObject2 : public QObject
{
    Q_OBJECT
public:
    explicit BusinessObject2(QObject *parent = 0);
public slots:
    void handleSomeSlotFinished(bool success);
};

#endif // BUSINESSOBJECT2_H
