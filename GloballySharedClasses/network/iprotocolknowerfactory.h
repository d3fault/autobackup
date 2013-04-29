#ifndef IPROTOCOLKNOWERFACTORY_H
#define IPROTOCOLKNOWERFACTORY_H

#include <QObject>

#include "iprotocolknower.h"

class IProtocolKnowerFactory : public QObject
{
    Q_OBJECT
public:
    explicit IProtocolKnowerFactory(QObject *parent) : QObject(parent) { }
    virtual IProtocolKnower *getNewProtocolKnower()=0; //TO DOnereq: caller takes ownership of it, so delete somewhere
};

#endif // IPROTOCOLKNOWERFACTORY_H
