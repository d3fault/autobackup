#ifndef CLEANROOM_H
#define CLEANROOM_H

#include <QObject>

#include "cleanroomcommon.h"
#include "cleanroomsession.h"

#define CleanRoomHandleNewSession handleNewSession

class CleanRoomDb;
class ICleanRoomSessionProvider;

class CleanRoom : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoom(QObject *parent = 0);
    void registerSessionProvider(ICleanRoomSessionProvider *cleanRoomSessionProvider);
private:    
    CleanRoomDb *m_Db;

    void showDefaultStaticallyLinkedPlugin(CleanRoomSession cleanRoomSession);
signals: //private
    void submitCleanRoomDocumentToDbBeginRequested(qint64 timestamp, const CleanRoomUsername &username, const CleanRoomLicenseShorthandIdentifier &licenseShorthandIdentifier, const QByteArray &data);
signals: //public
    void finishedSubmittingCleanRoomDocument(bool success, const QString &documentKey_aka_Sha1);
public slots:
    void submitCleanRoomDocumentBegin(qint64 timestamp, const CleanRoomUsername &username, const CleanRoomLicenseShorthandIdentifier &licenseShorthandIdentifier, const QByteArray &data);
private slots:
    void handleDbFinishedSubmittingCleanRoomDocument(bool success, const QString &documentKey_aka_Sha1);
    void CleanRoomHandleNewSession(CleanRoomSession cleanRoomSession);
};

#endif // CLEANROOM_H
