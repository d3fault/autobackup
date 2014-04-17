#ifndef LASTMODIFIEDTIMESTAMPSSORTERCLI_H
#define LASTMODIFIEDTIMESTAMPSSORTERCLI_H

#include <QObject>
#include <QTextStream>

class LastModifiedTimestampsSorter;

class LastModifiedTimestampsSorterCli : public QObject
{
    Q_OBJECT
public:
    explicit LastModifiedTimestampsSorterCli(QObject *parent = 0);
private:
    QTextStream m_StdOut;
    QTextStream m_StdErr;
    LastModifiedTimestampsSorter *m_LastModifiedTimestampsSorter;
private slots:
    void handleD(const QString &msg);
    void handleSortedLineOutput(const QString &sortedLineOutput);
};

#endif // LASTMODIFIEDTIMESTAMPSSORTERCLI_H
