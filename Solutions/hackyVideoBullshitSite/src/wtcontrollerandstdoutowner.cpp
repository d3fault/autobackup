#include "wtcontrollerandstdoutowner.h"

#include <fstream>

#include "backend/adimagewresource.h"

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
string WtControllerAndStdOutOwner::readFileIntoString(const char *filename)
{
    streampos fileSizeHack;
    char *fileBuffer;
    ifstream fileStream(filename, ios::in | ios::binary | ios::ate);
    if(fileStream.is_open())
    {
        fileSizeHack = fileStream.tellg();
        fileStream.seekg(0,ios::beg);
        fileBuffer = new char[fileSizeHack];
        fileStream.read(fileBuffer, fileSizeHack);
        fileStream.close();
    }
    else
    {
        handleE("failed to open " + QString::fromStdString(filename) + " for reading");
        return std::string("");
    }
    std::string ret = std::string(fileBuffer, fileSizeHack);
    delete [] fileBuffer;
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

    m_MySexyFaceLogoResource = new WFileResource("image/jpeg", "my.sexy.face.logo.jpg"); //TODOoptimization: 1 year expiration
    m_WtServer->addResource(m_MySexyFaceLogoResource, "/my.sexy.face.logo.jpg");

    //add the no ad global/public resource
    std::string noAdPlaceholderImageString = readFileIntoString("no.ad.placeholder.jpg");
    if(noAdPlaceholderImageString == "")
    {
        emit fatalErrorDetected();
        return;
    }
    m_NoAdImagePlaceholderResource = new AdImageWResource(noAdPlaceholderImageString, "image/jpeg", "image.jpg", WResource::Inline);
    m_WtServer->addResource(m_NoAdImagePlaceholderResource, "/no.ad.placeholder.jpg");

    std::string copyrightText = readFileIntoString("../../../copyright.txt"); //TODOoptional: settings file, cli arg, whatever
    if(copyrightText == "")
    {
        emit fatalErrorDetected();
        return;
    }
    HackyVideoBullshitSiteGUI::setCopyrightText(copyrightText);

    std::string dplLicenseText = readFileIntoString("../../../license.dpl.txt"); //eh redundant naming, unseen code is unseen, dgaf
    if(dplLicenseText == "")
    {
        emit fatalErrorDetected();
        return;
    }
    HackyVideoBullshitSiteGUI::setDplLicenseText(dplLicenseText);

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
