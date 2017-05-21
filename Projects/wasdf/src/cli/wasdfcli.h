#ifndef WASDFCLI_H
#define WASDFCLI_H

#include <QObject>

#include <QTextStream>

class WasdfCli : public QObject
{
    Q_OBJECT
public:
    explicit WasdfCli(QObject *parent = 0);
private:
    QTextStream m_StdErr;
    QTextStream m_StdOut;
private slots:
    void handleE(const QString &msg);
    void handleO(const QString &msg);
};

#endif // WASDFCLI_H
