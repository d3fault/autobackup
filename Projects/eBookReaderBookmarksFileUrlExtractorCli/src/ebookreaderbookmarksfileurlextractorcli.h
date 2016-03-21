#ifndef EBOOKREADERBOOKMARKSFILEURLEXTRACTORCLI_H
#define EBOOKREADERBOOKMARKSFILEURLEXTRACTORCLI_H

#include <QObject>

#include <QTextStream>

class eBookReaderBookmarksFileUrlExtractorCli : public QObject
{
    Q_OBJECT
public:
    explicit eBookReaderBookmarksFileUrlExtractorCli(QObject *parent = 0);
private:
    QTextStream m_StdOut;
private slots:
    void handleUrlExtracted(const QString &url);
};

#endif // EBOOKREADERBOOKMARKSFILEURLEXTRACTORCLI_H
