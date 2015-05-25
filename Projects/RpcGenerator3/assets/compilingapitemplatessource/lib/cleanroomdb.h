//THIS FILE IS NOT GENERATED BY RPC GENERATOR. IT IS ONLY TO GET MY AUTO-DRILLDOWN STUFF WORKING ASAP IN THE COMPILING TEMPLATE SOURCE
#ifndef CLEANROOMDB_H
#define CLEANROOMDB_H

#include <QObject>

#include <QMultiMap>

#include "cleanroomdoc.h"

class ICleanRoomFrontPageDefaultViewRequest;

class CleanRoomDb : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoomDb(QObject *parent = 0);

    void getLatestCleanRoomDocs(ICleanRoomFrontPageDefaultViewRequest *requestWeAreMerelyForwardingAroundInDbLikeUserDataAkaCookiePointer, int numLatestDocsToGet);
    void postCleanRoomDoc(QByteArray data);
private:
    //QSettings m_Db;
    //QVariantHash m_Db;
    //QSet<CleanRoomDoc> m_Db;
    QMultiMap<QDateTime, QByteArray> m_Db;
signals:
    void getLatestCleanRoomDocsFinished(ICleanRoomFrontPageDefaultViewRequest *requestWeAreMerelyForwardingAroundInDbLikeUserDataAkaCookiePointer, bool dbError, bool postCleanRoomDocSuccess_aka_LcbOp, QStringList latestCleanRoomDocs);
};

#endif // CLEANROOMDB_H
