#ifndef QTHTTPSERVERCLI_H
#define QTHTTPSERVERCLI_H

#include <QObject>
#include <QTextStream>

class QtHttpsServerCli : public QObject
{
    Q_OBJECT
public:
    explicit QtHttpsServerCli(QObject *parent = 0);
private:
    QTextStream m_StdErr;
private slots:
    void handleE(const QString &msg);
    void handleStandardInputLineReceived(const QString &standardInputLine);
    void handleQuitRequested();
};

#endif // QTHTTPSERVERCLI_H
