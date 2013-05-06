#include "recursivelycopytoemptydestinationandeasytreehashalongthewaybusiness.h"

RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWayBusiness::RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWayBusiness(QObject *parent) :
    QObject(parent)
{
    connect(&m_EasyTreeHasher, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
    connect(&m_EasyTreeHasher, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
}
void RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWayBusiness::recursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay(const QString &sourceDirectoryString, const QString &emptyDestinationDirectoryString, const QString &easyTreeHashOutputFilePath, QCryptographicHash::Algorithm algorithm)
{
    QDir sourceDirectory(sourceDirectoryString);
    QDir emptyDestinationDirectory(emptyDestinationDirectoryString);

    QFile easyTreeHashOutputFile(easyTreeHashOutputFilePath);

    m_EasyTreeHasher.recursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay(sourceDirectory, emptyDestinationDirectory, &easyTreeHashOutputFile, algorithm);

    emit copyOperationComplete();
}
