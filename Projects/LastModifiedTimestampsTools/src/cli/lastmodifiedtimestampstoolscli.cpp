#include "lastmodifiedtimestampstoolscli.h"

#include <QCoreApplication>
#include <QStringList>

#include "../lib/lastmodifiedtimestampstools.h"

LastModifiedTimestampsToolsCli::LastModifiedTimestampsToolsCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
{
    LastModifiedTimestampsTools *lastModifiedTimestampsTools = new LastModifiedTimestampsTools(this);
    connect(lastModifiedTimestampsTools, SIGNAL(d(QString)), this, SLOT(handleD(QString)));

    QStringList argz = qApp->arguments();
    if(argz.size() != 4)
    {
        usage();
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    QString action = argz.at(1).toLower();
    QString dir = argz.at(2);
    QString lastModifiedTimestampsFilePath = argz.at(3);
    if(action == "gen" || action == "generate")
    {
        connect(lastModifiedTimestampsTools, SIGNAL(lastModifiedTimestampsFileGenerated(bool)), qApp, SLOT(quit())); //maybe i should return ints instead of bools and connect to exit? reminds me of the C days lol (i prefer bools, and returning 0 for success fucks up my "if" logic by negating it, the fuckers), but eh would improve elegance/etc
        QMetaObject::invokeMethod(lastModifiedTimestampsTools, "beginGenerateLastModifiedTimestampsFile", Q_ARG(QString, dir), Q_ARG(QString, lastModifiedTimestampsFilePath));
        return;
    }
    if(action == "molest" || action == "touch")
    {
        connect(lastModifiedTimestampsTools, SIGNAL(heirarchyMolested(bool)), qApp, SLOT(quit()));
        QMetaObject::invokeMethod(lastModifiedTimestampsTools, "beginMolestHeirarchy", Q_ARG(QString, dir), Q_ARG(QString, lastModifiedTimestampsFilePath));
        return;
    }

    usage();
    handleD("Unknown action: " + action);
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
void LastModifiedTimestampsToolsCli::usage()
{
    QString usageStr =  "Usage: LastModifiedTimestampsToolsCli action dir lastModifiedTimestampsFile\n"
                        " Available Actions: generate, molest\n\n"
                        "  generate = generate last modified timestamps file based on the files in dir\n"
                        "  molest = change the timestamps of the files in dir to what is listed in last modified timestamps file\n";
    handleD(usageStr);
}
void LastModifiedTimestampsToolsCli::handleD(const QString &msg)
{
    m_StdOut << msg << endl;
}
