#ifndef STUPIDKEYVALUECONTENTTRACKER_H
#define STUPIDKEYVALUECONTENTTRACKER_H

#include <QObject>

class StupidKeyValueContentTracker : public QObject
{
    Q_OBJECT
public:
    explicit StupidKeyValueContentTracker(QObject *parent = 0);
signals:
    void e(const QString &msg);
    void o(const QString &msg);

    void finishedAdd();
    void finishedCommit(bool success);
public slots:
    void add(const QByteArray &key, const QByteArray &data);
    void commit(const QString &commitMessage);
};

#endif // STUPIDKEYVALUECONTENTTRACKER_H
