#include "lastmodifiedtimestampssortercli.h"

#include <QCoreApplication>
#include <QStringList>

#include "../lib/lastmodifiedtimestampssorter.h"

LastModifiedTimestampsSorterCli::LastModifiedTimestampsSorterCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
    , m_LastModifiedTimestampsSorter(0)
{
    QStringList args = QCoreApplication::arguments();
    if(args.size() != 2)
    {
        m_StdOut << "usage:" << endl << "LastModifiedTimestampsSorterCli lastModifiedTimestampsFile" << endl;
        QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
        return;
    }
    m_LastModifiedTimestampsSorter = new LastModifiedTimestampsSorter(this);
    connect(m_LastModifiedTimestampsSorter, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
    connect(m_LastModifiedTimestampsSorter, SIGNAL(sortedLineOutput(QString)), this, SLOT(handleSortedLineOutput(QString)));
    connect(m_LastModifiedTimestampsSorter, SIGNAL(finishedOutputtingSortedLines(int)), QCoreApplication::instance(), SLOT(quit()), Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_LastModifiedTimestampsSorter, "sortAndOutputLastModifiedTimestamps", Q_ARG(QString,args.at(1)));
}
void LastModifiedTimestampsSorterCli::handleD(const QString &msg)
{
    m_StdErr << msg << endl;
}
void LastModifiedTimestampsSorterCli::handleSortedLineOutput(const QString &sortedLineOutput)
{
    m_StdOut << sortedLineOutput << endl;
}
