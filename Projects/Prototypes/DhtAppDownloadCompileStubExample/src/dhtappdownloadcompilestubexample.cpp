#include "dhtappdownloadcompilestubexample.h"

#include <QList>
#include <QProcess>
#include <QFileInfo>

#include "dht.h"

//Generator fills in these three values and that's it
#define PROJECT_HUMAN_READABLE_NAME "Example Project" //setWindowTitle of window shown when p2p is downloading (progress bar at very least)
#define PROJECT_FS_NAME_OR_TARGET "exampleproject"
#define PROJECT_SHA1 "5465309a42d19935cba7aa4accab9cbed75415ed"
#define PROJECT_BOOTSTRAP_IP_ADDRESSES "0.0.0.0,0.0.0.1,0.0.0.2,0.0.0.3"; //comma separated, or just that qlist idfk

#define DhtAppDownloadCompileStubExample_CALL_TEH_PROCESS(tehProcess) \
process.start(tehProcess); \
if(!process.waitForStarted()) \
{ \
    emit e("error starting " + tehProcess); \
    return; \
} \
if(!process.waitForFinished(-1)) \
{ \
    emit e(tehProcess + " failed to finish"); \
    return; \
} \
if(process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) \
{ \
    emit e(tehProcess + " finished irregularly"); \
    return; \
}

//The idea is that if everybody uses this stub, its own size becomes negligible. It can single handedly organize all windows libs, but that's getting off-topic about how much windows sucks
DhtAppDownloadCompileStubExample::DhtAppDownloadCompileStubExample(QObject *parent)
    : QObject(parent)
    , m_Dht(0)
{
    //connect to dht using [preseeded-selected-known-to-recently-have-had-a-good-online-track-record] peers,
    //download payload for name/identifier (TODOoptional: authenticate. sha1 provides pretty good abilities already)
    //compile
    //execute
    stubFirstRun();
}
DhtAppDownloadCompileStubExample::~DhtAppDownloadCompileStubExample()
{
    if(m_Dht)
        delete m_Dht;
}
QString DhtAppDownloadCompileStubExample::binaryToFilesystem(const QByteArray &binary)
{
    //*cough*tarball*cough*

    QString directoryExtractedBecauseWeWantToRunQmakeMakeInIt; //populate during this method's business impl
    return directoryExtractedBecauseWeWantToRunQmakeMakeInIt;
}
void DhtAppDownloadCompileStubExample::stubFirstRun()
{
    QList<QString> hardcodedIntoBinaryListOfIps; //hell, we could store thousands quite cheaply
    hardcodedIntoBinaryListOfIps << "393.111.222.444" << "393.339.574.393" << "etc i am making these up"; //after first run, you use a different set (unless your session data was deleted ofc, then you use the same bootstrap IPs again)... just to spread load. TODOreq: put in separate file that is generated using random ips and linked into binary using qrc
    //QString sha1ofA7zThatIsKnownToBeOnTheP2pNetwork; //TODOreq: the generator changes the "display name" and this sha1 and that's all [it needs to]. ok i want to add "fs name aka target name" so i can pass it to qprocess
    QString fsName_AkaTargetName(PROJECT_FS_NAME_OR_TARGET); //My Binary Name Goes Here is human readable variant
    QByteArray the7zMatchingTheSha1WeSupplied = dhtDownload(hardcodedIntoBinaryListOfIps, PROJECT_SHA1); //extract implied
    QString outputAbsolutePathOfBinary = compile(binaryToFilesystem(qUncompress(the7zMatchingTheSha1WeSupplied)));
    QProcess process;
    QFileInfo binaryFileInfo(fsName_AkaTargetName);
    process.setWorkingDirectory(binaryFileInfo.absolutePath());
    DhtAppDownloadCompileStubExample_CALL_TEH_PROCESS(fsName_AkaTargetName)
}
QByteArray DhtAppDownloadCompileStubExample::dhtDownload(QList<QString> dhtSeedIpAddresses, const QString &sha1ofA7zThatIsKnownToBeOnTheP2pNetwork)
{
    if(!m_Dht)
    {
        m_Dht = new Dht(dhtSeedIpAddresses); //connect implied i suppose, but error checking could be performed w/e

        //i put it on the heap because i want it to run in the background even after we are done downloading the target blob (that's how p2p works)
    }
    QByteArray download = m_Dht->downloadBlob(sha1ofA7zThatIsKnownToBeOnTheP2pNetwork);
    return download;
}
QString DhtAppDownloadCompileStubExample::compile(const QString &dirToRunQmakeMakeIn_ItWasJustUn7zadAfterDlingFromP2p)
{
    QProcess process;
    process.setWorkingDirectory(dirToRunQmakeMakeIn_ItWasJustUn7zadAfterDlingFromP2p);
    DhtAppDownloadCompileStubExample_CALL_TEH_PROCESS("qmake")
    DhtAppDownloadCompileStubExample_CALL_TEH_PROCESS("make")
    return embeddedIntoStubLoLTargetDefaultFilenameForCallingButReallyWeShouldKnowItEvenIfTheyChangeItBlah;
}
