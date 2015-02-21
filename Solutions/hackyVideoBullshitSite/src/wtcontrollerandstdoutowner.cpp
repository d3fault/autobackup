#include "wtcontrollerandstdoutowner.h"

#include <boost/filesystem.hpp>

#include <Wt/WMemoryResource>
#include <Wt/WFileResource>

#include <QFile>

#include "frontend/hvbsshared.h"
#include "backend/nonexpiringstringwresource.h"


#define HVBS_CREATE_STATIC_PUBLIC_WT_FILE_RESOURCE(resourceStackVarPrefix, filename, staticPublicResourceUrl) \
if(!boost::filesystem::exists(filename)) \
{ \
    handleE(filename " does not exist. quitting"); \
    emit fatalErrorDetected(); \
    return; \
} \
WFileResource resourceStackVarPrefix##FileResource("application/x-bittorrent", filename); \
resourceStackVarPrefix##FileResource.suggestFileName(filename); \
m_WtServer->addResource(&resourceStackVarPrefix##FileResource, staticPublicResourceUrl);

//this thread owns stdout because i can't (well...) tell wt to not write to stdout at any given moment...
//i can't for the life of me figure out if this is a backend or frontend class :-P
void WtControllerAndStdOutOwner::setAdImageGetAndSubscribeManager(AdImageGetAndSubscribeManager *adImageGetAndSubscribeManager)
{
    HackyVideoBullshitSiteGUI::setAdImageGetAndSubscribeManager(adImageGetAndSubscribeManager);
}
void WtControllerAndStdOutOwner::setAirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL(const QString &airborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL)
{
    HackyVideoBullshitSiteGUI::setAirborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL(airborneVideoSegmentsBaseDirActual_NOT_CLEAN_URL);
}
void WtControllerAndStdOutOwner::setMyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended(const QString &myBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended)
{
    HackyVideoBullshitSiteGUI::setMyBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended(myBrainArchiveBaseDirActual_NOT_CLEAN_URL_NoSlashAppended);
}
void WtControllerAndStdOutOwner::setTimestampsAndPathsSharedAtomicPointer(QAtomicPointer<LastModifiedTimestampsAndPaths> *lastModifiedTimestampsSharedAtomicPointer)
{
    HackyVideoBullshitSiteGUI::setTimestampsAndPathsSharedAtomicPointer(lastModifiedTimestampsSharedAtomicPointer);
}
WtControllerAndStdOutOwner::WtControllerAndStdOutOwner(QObject *parent)
    : QObject(parent)
    , m_WtServer(0) //still can't pass my objects on threads args guh :(
    , m_MySexyFaceLogoResource(0)
    , m_NoAdImagePlaceholderResource(0)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{ }
WtControllerAndStdOutOwner::~WtControllerAndStdOutOwner()
{
    if(m_MySexyFaceLogoResource)
        delete m_MySexyFaceLogoResource;
    if(m_NoAdImagePlaceholderResource)
        delete m_NoAdImagePlaceholderResource;
    if(m_WtServer)
    {
        if(m_WtServer->isRunning())
        {
            m_WtServer->stop();
        }
        delete m_WtServer;
    }
}
string WtControllerAndStdOutOwner::readFileIntoString(const QString &filename)
{
    QFile theFile(filename);
    if(!theFile.open(QIODevice::ReadOnly))
    {
        handleE("failed to open " + filename + " for reading");
        return std::string("");
    }
    QByteArray theFileBA = theFile.readAll();
    std::string ret(theFileBA.constData(), theFileBA.length());
    return ret;
}
void WtControllerAndStdOutOwner::initializeAndStart(int argc, char **argv)
{
    if(m_MySexyFaceLogoResource)
    {
        delete m_MySexyFaceLogoResource;
        m_MySexyFaceLogoResource = 0;
    }
    if(m_NoAdImagePlaceholderResource)
    {
        delete m_NoAdImagePlaceholderResource;
        m_NoAdImagePlaceholderResource = 0;
    }
    if(m_WtServer)
    {
        if(m_WtServer->isRunning())
            m_WtServer->stop();
        delete m_WtServer;
    }
    m_WtServer = new WServer(argv[0]);
    m_WtServer->setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
    m_WtServer->addEntryPoint(Application, &HackyVideoBullshitSiteGUI::hackyVideoBullshitSiteGuiEntryPoint);

    std::string mySexyFaceLogoString = readFileIntoString(":/my.sexy.face.logo.jpg");
    if(mySexyFaceLogoString == "")
    {
        emit fatalErrorDetected();
        return;
    }
    m_MySexyFaceLogoResource = new NonExpiringStringWResource(mySexyFaceLogoString, "image/jpeg", "my.sexy.face.logo.jpg", WResource::Inline);
    m_WtServer->addResource(m_MySexyFaceLogoResource, "/my.sexy.face.logo.jpg");

    //add the no ad global/public resource
    std::string noAdPlaceholderImageString = readFileIntoString(":/no.ad.placeholder.jpg");
    if(noAdPlaceholderImageString == "")
    {
        emit fatalErrorDetected();
        return;
    }
    m_NoAdImagePlaceholderResource = new NonExpiringStringWResource(noAdPlaceholderImageString, "image/jpeg", "image.jpg", WResource::Inline);
    m_WtServer->addResource(m_NoAdImagePlaceholderResource, "/no.ad.placeholder.jpg");

    std::string copyrightText = readFileIntoString(":/copyright.txt"); //TODOoptional: settings file, cli arg, whatever
    if(copyrightText == "")
    {
        emit fatalErrorDetected();
        return;
    }
    HackyVideoBullshitSiteGUI::setCopyrightText(copyrightText);

    std::string dplLicenseText = readFileIntoString(":/license.dpl.txt"); //eh redundant naming, unseen code is unseen, dgaf
    if(dplLicenseText == "")
    {
        emit fatalErrorDetected();
        return;
    }
    HackyVideoBullshitSiteGUI::setDplLicenseText(dplLicenseText);

    //BEGIN TORRENTS
    HVBS_CREATE_STATIC_PUBLIC_WT_FILE_RESOURCE(myBrainPublicFilesTorrent, HVBS_MY_BRAIN_PUBLIC_FILES_TORRENT_FILENAME, HVBS_MY_BRAIN_PUBLIC_FILES_TORRENT_PUBLIC_RESOURCE)
    HVBS_CREATE_STATIC_PUBLIC_WT_FILE_RESOURCE(myBrain2014supplementTorrent, HVBS_MY_BRAIN_2014_SUPPLEMENT_TORRENT_FILENAME, HVBS_MY_BRAIN_2014_SUPPLEMENT_TORRENT_PUBLIC_RESOURCE)
    HVBS_CREATE_STATIC_PUBLIC_WT_FILE_RESOURCE(myBrainPrivateFilesTorrent, HVBS_MY_BRAIN_PRIVATE_FILES_TORRENT_FILENAME, HVBS_MY_BRAIN_PRIVATE_FILES_TORRENT_PUBLIC_RESOURCE)
    //END TORRENTS

    if(!m_WtServer->start())
    {
        handleE("failed to start wt server");
        emit fatalErrorDetected();
        return;
    }
    handleO("wt server started");
    emit started();
}
void WtControllerAndStdOutOwner::stop()
{
    if(m_WtServer)
        m_WtServer->stop();
    emit stopped();
}
void WtControllerAndStdOutOwner::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void WtControllerAndStdOutOwner::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
