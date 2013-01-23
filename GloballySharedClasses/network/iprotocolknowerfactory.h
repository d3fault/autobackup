#ifndef IPROTOCOLKNOWERFACTORY_H
#define IPROTOCOLKNOWERFACTORY_H

#include <QObject>

#include "iprotocolknower.h"

class IProtocolKnowerFactory : public QObject
{
    Q_OBJECT
public:
    explicit void IProtocolKnowerFactory(QObject *parent) : QObject(parent) { }
protected:
    virtual IProtocolKnower *getNewProtocolKnower()=0; //TODOreq: caller takes ownership of it, so delete somewhere
};

#endif // IPROTOCOLKNOWERFACTORY_H
