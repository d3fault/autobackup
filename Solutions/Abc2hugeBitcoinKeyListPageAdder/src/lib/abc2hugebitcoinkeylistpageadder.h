#ifndef ABC2HUGEBITCOINKEYLISTPAGEADDER_H
#define ABC2HUGEBITCOINKEYLISTPAGEADDER_H

#include <QObject>
#include "isynchronouslibcouchbaseuser.h"

class Abc2hugeBitcoinKeyListPageAdder : public QObject, public ISynchronousLibCouchbaseUser
{
    Q_OBJECT
public:
    explicit Abc2hugeBitcoinKeyListPageAdder(QObject *parent = 0);
private:
    void errorOutput(const string &errorString);
signals:
    void d(const QString &);
    void doneAddingHugeBitcoinKeyListPages();
public slots:
    void addHugeBitcoinKeyListPages(const QString &nonExistentStartingPageIndex, const QString &filenameOfBitcoinKeysIn10kIncrements);
};

#endif // ABC2HUGEBITCOINKEYLISTPAGEADDER_H
