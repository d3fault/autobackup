#ifndef WTCONTROLLERANDSTDOUTOWNER_H
#define WTCONTROLLERANDSTDOUTOWNER_H

#include <Wt/WServer>
using namespace Wt;

#include <QObject>
#include <QTextStream>

#include "frontend/hackyvideobullshitsitegui.h"

class WtControllerAndStdOutOwner : public QObject
{
    Q_OBJECT
public:
    static void setAdImageGetAndSubscribeManager(AdImageGetAndSubscribeManager* adImageGetAndSubscribeManager);
    static void setAirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL(const QString &airborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL);
    static void setMyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended(const QString &myBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended);
    static void setTimestampsAndPathsSharedAtomicPointer(QAtomicPointer<LastModifiedTimestampsAndPaths> *lastModifiedTimestampsSharedAtomicPointer);

    explicit WtControllerAndStdOutOwner(QObject *parent = 0);
    ~WtControllerAndStdOutOwner();
private:
    WServer *m_WtServer;
    NonExpiringStringWResource *m_MySexyFaceLogoResource;
    NonExpiringStringWResource *m_NoAdImagePlaceholderResource;

    QTextStream m_StdOut;
    QTextStream m_StdErr;

    std::string readFileIntoString(const QString &filenam);
signals:
    void fatalErrorDetected();
    void started();
    void stopped();
public slots:
    void initializeAndStart(int argc, char **argv);
    void stop();
    void handleO(const QString &msg);
    void handleE(const QString &msg);
};

#endif // WTCONTROLLERANDSTDOUTOWNER_H
