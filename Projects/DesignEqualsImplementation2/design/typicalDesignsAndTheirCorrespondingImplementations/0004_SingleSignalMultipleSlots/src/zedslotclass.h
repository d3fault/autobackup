#ifndef ZEDSLOTCLASS_H
#define ZEDSLOTCLASS_H

#include <QObject>

class ZedSlotClass : public QObject
{
    Q_OBJECT
public slots:
    void zedSlot();
};

#endif // ZEDSLOTCLASS_H
