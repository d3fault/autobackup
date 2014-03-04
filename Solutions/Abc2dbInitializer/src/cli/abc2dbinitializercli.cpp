#include "abc2dbinitializercli.h"

#include <QStringList>

Abc2dbInitializerCli::Abc2dbInitializerCli(QObject *parent) :
    QObject(parent), m_StdOut(stdout)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleAbc2dbInitializerReadyForConnectioins()));
    m_BusinessThread.start();
}
void Abc2dbInitializerCli::handleD(const QString &msg)
{
    m_StdOut << msg << endl;
}
void Abc2dbInitializerCli::handleAbc2dbInitializerReadyForConnectioins()
{
    Abc2dbInitializer *abc2dbInitializer = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(abc2dbInitializer, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
    connect(this, SIGNAL(initializeAbc2dbRequested(QString)), abc2dbInitializer, SLOT(initializeAbc2db(QString)));
    connect(abc2dbInitializer, SIGNAL(doneInitializingAbc2db()), QCoreApplication::instance(), SLOT(quit()));

    //derp, qt cli needs some love. sure it works, but it isn't as clean/fun/smechzy as gui land
    QStringList tehArgs = QCoreApplication::arguments();
    if(tehArgs.size() != 2)
    {
        handleD("usage: " + QCoreApplication::applicationFilePath() + " enormousBitcoinKeyListFile");
        handleD("");
        handleD("enormous bitcoin key list file has one key on each line and must have AT LEAST 110 thousand bitcoin keys in it. you can add more, but you must do so in exactly 10 thousand key increments. failing to do so will fuck shit up royally");
        QCoreApplication::quit();
        return;
    }
    emit initializeAbc2dbRequested(tehArgs.at(1));
}
void Abc2dbInitializerCli::handleAboutToQuit() //derp fucking cli doesn't even get here on ctrl+c, fucking 2014
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}
