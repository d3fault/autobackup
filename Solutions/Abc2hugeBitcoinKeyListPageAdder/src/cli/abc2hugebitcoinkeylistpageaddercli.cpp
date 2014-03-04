#include "abc2hugebitcoinkeylistpageaddercli.h"

#include <QStringList>

//OT: cli apps are far likelier to be chained together, so saying that each app should use my usual 'backend thread' method is really just an unnecessary performance penalty for such situations. i've vaguely touched on the subject before, but... 10k console apps chained together synchronously/single-threaded goes an order of magnitude faster than 10k console apps chained together, where each one of those 10k has to make, jump to, and clean up a new thread. still looking for that slick codan paradigm that applies to gui/cli (fucking about to quit is shit), starting to think i have to make it. even further OT: i'd rather do designEquals type shit and just glue the c/c++ manually for that than work on such low level stuff (although i do desire both). i'm sent to this world to abstract upon it. surfing existing low level is tediously fun/shit
Abc2hugeBitcoinKeyListPageAdderCli::Abc2hugeBitcoinKeyListPageAdderCli(QObject *parent) :
    QObject(parent), m_StdOut(stdout)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));
    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleAbc2hugeBitcoinKeyListPageAdderReadyForConnections()));
    m_BusinessThread.start();
}
void Abc2hugeBitcoinKeyListPageAdderCli::handleD(const QString &msg)
{
    m_StdOut << msg << endl;
}
void Abc2hugeBitcoinKeyListPageAdderCli::handleAbc2hugeBitcoinKeyListPageAdderReadyForConnections()
{
    Abc2hugeBitcoinKeyListPageAdder *abc2hugeBitcoinKeyListPageAdder = m_BusinessThread.getObjectPointerForConnectionsOnly();
    connect(abc2hugeBitcoinKeyListPageAdder, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
    connect(this, SIGNAL(addHugeBitcoinKeyListPagesRequested(QString,QString)), abc2hugeBitcoinKeyListPageAdder, SLOT(addHugeBitcoinKeyListPages(QString,QString)));
    connect(abc2hugeBitcoinKeyListPageAdder, SIGNAL(doneAddingHugeBitcoinKeyListPages()), QCoreApplication::instance(), SLOT(quit()));

    QStringList tehArgs = QCoreApplication::arguments();
    if(tehArgs.size() != 3)
    {
        handleD("usage: " + QCoreApplication::applicationFilePath() + " pageIndex bitcoinKeyListPagesFile");
        handleD("");
        handleD("pageIndex must not exist, else you will get an error. if pageIndex is not correct, you will still get out of keys errors");
        handleD("bitcoinKeyListPagesFile has one key on each line and must have an exact multiple of 10 thousand keys. failing to do so will fuck shit up royally");
        QCoreApplication::quit();
        return;
    }

    emit addHugeBitcoinKeyListPagesRequested(tehArgs.at(1), tehArgs.at(2));
}
void Abc2hugeBitcoinKeyListPageAdderCli::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}
