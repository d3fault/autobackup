#ifndef ABC2PESSIMISTICSTATEMONITORANDRECOVERER_H
#define ABC2PESSIMISTICSTATEMONITORANDRECOVERER_H

#include "isynchronouslibcouchbaseuser.h"

#include <string>
using namespace std;

class Abc2PessimisticStateMonitorAndRecoverer : public ISynchronousLibCouchbaseUser
{
public:
    Abc2PessimisticStateMonitorAndRecoverer();
    int startPessimisticallyMonitoringAndRecovereringStateUntilToldToStop();
private:
    void errorOutput(const string &errorString);
};

#endif // ABC2PESSIMISTICSTATEMONITORANDRECOVERER_H
