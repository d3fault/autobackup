#ifndef IMESSAGEDISPENSER_H
#define IMESSAGEDISPENSER_H

#include <QObject>

#include "../irpcclientshelper.h"

class IMessageDispenser : public QObject
{
    Q_OBJECT
public:
    explicit IMessageDispenser(QObject *mandatoryParent);
    void setDestinationObject(IRpcBusinessController *destinationObject);
private:
    IRpcBusinessController *m_DestinationObject;
signals:

public slots:

};

#endif // IMESSAGEDISPENSER_H
