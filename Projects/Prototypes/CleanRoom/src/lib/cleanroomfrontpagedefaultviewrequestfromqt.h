#ifndef CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
#define CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H

#include <QObject>
#include "ifrontpagedefaultviewrequest.h"

class CleanRoomSession;

class CleanRoomFrontPageDefaultViewRequestFromQt : public IFrontPageDefaultViewRequest
{
    Q_OBJECT
public:
    CleanRoomFrontPageDefaultViewRequestFromQt(CleanRoom *cleanRoom, QObject *objectCallback, const QString &slotCallback, QObject *parent = 0);
    //void regainContextPossiblyInOtherThread_aka_respondActual(QVariantList responseArgs);
    void respond(QStringList frontPageDocs);
private:
    //QObject *m_ObjectCallback;
    //std::string m_SlotCallback;
signals:
    void frontPageDefaultViewResponseRequested(QStringList frontPageDocs);
};

#endif // CLEANROOMFRONTPAGEDEFAULTVIEWREQUESTFROMQT_H
