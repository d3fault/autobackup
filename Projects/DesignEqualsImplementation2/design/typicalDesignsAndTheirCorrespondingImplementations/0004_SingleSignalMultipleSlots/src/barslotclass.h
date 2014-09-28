#ifndef BARSLOTCLASS_H
#define BARSLOTCLASS_H

#include <QObject>

class BarSlotClass : public QObject
{
    Q_OBJECT
public slots:
    void barSlot0();
    void barSlot1();
};

#endif // BARSLOTCLASS_H
