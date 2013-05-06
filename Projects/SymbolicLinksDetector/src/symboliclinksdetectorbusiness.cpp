#include "symboliclinksdetectorbusiness.h"

SymbolicLinksDetectorBusiness::SymbolicLinksDetectorBusiness(QObject *parent) :
    QObject(parent)
{
    connect(&m_SymbolicLinksDetector, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void SymbolicLinksDetectorBusiness::detectSymbolicLinks(const QString &searchDirectoryString, bool quitOnFirstFound_TRUE__OR__listEachOnesPath_FALSE)
{
    QDir searchDirectoryDir(searchDirectoryString);
    m_SymbolicLinksDetector.detectSymbolicLinks(searchDirectoryDir, quitOnFirstFound_TRUE__OR__listEachOnesPath_FALSE);
}
