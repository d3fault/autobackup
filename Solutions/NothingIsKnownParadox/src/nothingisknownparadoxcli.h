#ifndef NOTHINGISKNOWNPARADOXCLI_H
#define NOTHINGISKNOWNPARADOXCLI_H

#include <QObject>

#include <QTextStream>

class NothingIsKnownParadoxCli : public QObject
{
    Q_OBJECT
public:
    explicit NothingIsKnownParadoxCli(QObject *parent = 0);
private:
    QTextStream m_StdOut;
private slots:
    void handleO(const QString &msg);
};

#endif // NOTHINGISKNOWNPARADOXCLI_H
