#ifndef ABC2DBINITIALIZER_H
#define ABC2DBINITIALIZER_H

#include <QObject>
#include "isynchronouslibcouchbaseuser.h"

class Abc2dbInitializer : public QObject, public ISynchronousLibCouchbaseUser
{
    Q_OBJECT
public:
    explicit Abc2dbInitializer(QObject *parent = 0);
private:
    void errorOutput(const string &errorString);
signals:
    void d(const QString &);
    void doneInitializingAbc2db();
public slots:
    void initializeAbc2db(const QString &filenameOfLineSeparatedEnormousBitcoinKeyListThatHasAtLeast_110k_keys_ButCanBeMoreIn_10k_key_increments);
};

#endif // ABC2DBINITIALIZER_H
