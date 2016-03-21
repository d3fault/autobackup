#include "ebookreaderbookmarksfileurlextractorcli.h"

#include <QCoreApplication>
#include <QStringList>

#include "ebookreaderbookmarksfileurlextractor.h"

eBookReaderBookmarksFileUrlExtractorCli::eBookReaderBookmarksFileUrlExtractorCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
{
    QStringList argz = qApp->arguments();
    QString appName = argz.takeFirst(); //app name
    if(argz.size() != 1)
    {
        m_StdOut << "Usage:" << appName << " bookmarksFilePath" << endl;
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    QString bookmarksFilePath = argz.first();

    eBookReaderBookmarksFileUrlExtractor *urlExtractor = new eBookReaderBookmarksFileUrlExtractor(this);
    connect(urlExtractor, SIGNAL(urlExtracted(QString)), this, SLOT(handleUrlExtracted(QString)));
    connect(urlExtractor, SIGNAL(finishedExtractingUrlsFromEbookReaderBookmarksFile(bool)), qApp, SLOT(quit()), Qt::QueuedConnection);
    QMetaObject::invokeMethod(urlExtractor, "startExtractingUrlsFromEbookReaderBookmarksFile", Q_ARG(QString, bookmarksFilePath));
}
void eBookReaderBookmarksFileUrlExtractorCli::handleUrlExtracted(const QString &url)
{
    m_StdOut << url << endl;
}
