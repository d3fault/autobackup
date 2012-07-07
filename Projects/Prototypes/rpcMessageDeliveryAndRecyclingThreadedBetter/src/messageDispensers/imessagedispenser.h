#ifndef IMESSAGEDISPENSER_H
#define IMESSAGEDISPENSER_H

#include <QObject>

class IMessageDispenser : public QObject
{
    Q_OBJECT
public:
    explicit IMessageDispenser(QObject *mandatoryOwnerBeforeOwnerHasBeenMoveToThreaded);

signals:

public slots:

};

#endif // IMESSAGEDISPENSER_H
