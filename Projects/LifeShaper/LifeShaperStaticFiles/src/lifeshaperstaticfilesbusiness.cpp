#include "lifeshaperstaticfilesbusiness.h"

LifeShaperStaticFilesBusiness::LifeShaperStaticFilesBusiness(QObject *parent) :
    QObject(parent)
{
    connect(&m_LifeShaperStaticFiles, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(&m_LifeShaperStaticFiles, SIGNAL(nowProcessingEasyTreeHashItem(QString,QString,bool,qint64,QString,QString)), this, SIGNAL(nowProcessingEasyTreeHashItem(QString,QString,bool,qint64,QString,QString)));
    connect(&m_LifeShaperStaticFiles, SIGNAL(stoppedRecursingDatOperationBecauseParentDirEntered()), this, SIGNAL(stoppedRecursingDatOperationBecauseParentDirEntered()));
    connect(&m_LifeShaperStaticFiles, SIGNAL(finishedProcessingEasyTreeHashFile()), this, SIGNAL(finishedProcessingEasyTreeHashFile()));
}
LifeShaperStaticFilesBusiness::~LifeShaperStaticFilesBusiness()
{
    cleanUp();
}
void LifeShaperStaticFilesBusiness::cleanUp()
{
    if(m_EasyTreeHashFile.isOpen())
    {
        m_EasyTreeHashFile.close();
    }
}
void LifeShaperStaticFilesBusiness::startIteratingEasyTreeHashFile(QString easyTreeHashFile, QString outputFilesFolder, QString lineLeftOffFrom_OR_emptyString)
{
    cleanUp();
    m_EasyTreeHashFile.setFileName(easyTreeHashFile);
    if(m_EasyTreeHashFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m_LifeShaperStaticFiles.startIteratingEasyTreeHashFile(&m_EasyTreeHashFile, outputFilesFolder, lineLeftOffFrom_OR_emptyString);
    }
    else
    {
        emit d("failed to open input file");
    }
}
void LifeShaperStaticFilesBusiness::stopIteratingEasyTreeHashFile()
{
    m_LifeShaperStaticFiles.stopIteratingEasyTreeHashFile();
    cleanUp();
}
void LifeShaperStaticFilesBusiness::setRecursivelyApplyFromHereUntilParent(bool recursiveEnabled)
{
    m_LifeShaperStaticFiles.setRecursivelyApplyFromHereUntilParent(recursiveEnabled);
}
void LifeShaperStaticFilesBusiness::leaveBehind()
{
    m_LifeShaperStaticFiles.leaveBehind();
}
void LifeShaperStaticFilesBusiness::iffyCopyright()
{
    m_LifeShaperStaticFiles.iffyCopyright();
}
void LifeShaperStaticFilesBusiness::deferDecision()
{
    m_LifeShaperStaticFiles.deferDecision();
}
void LifeShaperStaticFilesBusiness::bringForward()
{
    m_LifeShaperStaticFiles.bringForward();
}
void LifeShaperStaticFilesBusiness::useTHISasReplacement(QString replacementFilePath)
{
    m_LifeShaperStaticFiles.useTHISasReplacement(replacementFilePath);
}
