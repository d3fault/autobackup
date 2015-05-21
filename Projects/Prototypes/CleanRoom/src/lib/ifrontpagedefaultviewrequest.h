#ifndef IFRONTPAGEDEFAULTVIEWREQUEST_H
#define IFRONTPAGEDEFAULTVIEWREQUEST_H

#include "icleanroomrequest.h"

//#include <QVariantList>

#include "cleanroom.h"

class CleanRoom;

class IFrontPageDefaultViewRequest : public QObject, public ICleanRoomRequest
{
    Q_OBJECT
public:
    IFrontPageDefaultViewRequest(CleanRoom *cleanRoom, QObject *parent = 0)
        : QObject(parent)
        , ICleanRoomRequest(cleanRoom)
    {
        connect(this, SIGNAL(frontPageDefaultViewRequested(IFrontPageDefaultViewRequest*)), cleanRoom, SLOT(getFrontPageDefaultView(IFrontPageDefaultViewRequest*)));
    }
    void invokeSlotThatHandlesRequest();
    virtual void respond(QStringList frontPageDocs)=0;
#if 0
    void convertResponseArgsFromVariantListToApiSpecificTypes/*_AndThenCallThisSlotWithThemAwwShitNvmIcantBoostWtThatWay*/(QVariantList responseArgsAsVariantList)
    {
        //the rpc generator 'knows' which list items are which response args
        QStringList frontPageDocs = responseArgsAsVariantList.first();
    }
#endif
signals:
    void frontPageDefaultViewRequested(IFrontPageDefaultViewRequest *request);
};

#endif // IFRONTPAGEDEFAULTVIEWREQUEST_H
