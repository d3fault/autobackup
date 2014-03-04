#ifndef ABC2DBINITIALIZERCLI_H
#define ABC2DBINITIALIZERCLI_H

#include <QCoreApplication>
#include <QObject>
#include <QTextStream>

#include "objectonthreadhelper.h"
#include "abc2dbinitializer.h"

class Abc2dbInitializerCli : public QObject
{
    Q_OBJECT
public:
    explicit Abc2dbInitializerCli(QObject *parent = 0);
private:
    ObjectOnThreadHelper<Abc2dbInitializer> m_BusinessThread;
    QTextStream m_StdOut;
signals:
    void initializeAbc2dbRequested(const QString &);
private slots:
    void handleD(const QString &msg);
    void handleAbc2dbInitializerReadyForConnectioins();
    void handleAboutToQuit();
};

#endif // ABC2DBINITIALIZERCLI_H
