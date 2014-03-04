#ifndef ABC2HUGEBITCOINKEYLISTPAGEADDERCLI_H
#define ABC2HUGEBITCOINKEYLISTPAGEADDERCLI_H

#include <QCoreApplication>
#include <QObject>
#include <QTextStream>

#include "objectonthreadhelper.h"
#include "abc2hugebitcoinkeylistpageadder.h"

class Abc2hugeBitcoinKeyListPageAdderCli : public QObject
{
    Q_OBJECT
public:
    explicit Abc2hugeBitcoinKeyListPageAdderCli(QObject *parent = 0);
private:
    ObjectOnThreadHelper<Abc2hugeBitcoinKeyListPageAdder> m_BusinessThread;
    QTextStream m_StdOut;
signals:
    void addHugeBitcoinKeyListPagesRequested(const QString &nonExistentStartingPageIndex, const QString &filenameOfBitcoinKeysIn10kIncrements);
private slots:
    void handleD(const QString &msg);
    void handleAbc2hugeBitcoinKeyListPageAdderReadyForConnections();
    void handleAboutToQuit();
};

#endif // ABC2HUGEBITCOINKEYLISTPAGEADDERCLI_H
