#ifndef SINGLESIGNALMULTIPLESLOTSCLI_H
#define SINGLESIGNALMULTIPLESLOTSCLI_H

#include <QObject>

class SingleSignalMultipleSlotsCli : public QObject
{
    Q_OBJECT
public:
    explicit SingleSignalMultipleSlotsCli(QObject *parent = 0);
};

#endif // SINGLESIGNALMULTIPLESLOTSCLI_H
