#ifndef LASTMODIFIEDTIMESTAMPSTOOLSCLI_H
#define LASTMODIFIEDTIMESTAMPSTOOLSCLI_H

#include <QObject>
#include <QTextStream>

class LastModifiedTimestampsToolsCli : public QObject
{
    Q_OBJECT
public:
    explicit LastModifiedTimestampsToolsCli(QObject *parent = 0);
private:
    QTextStream m_StdOut;
    void usage();
private slots:
    void handleD(const QString &msg);
};

#endif // LASTMODIFIEDTIMESTAMPSTOOLSCLI_H
