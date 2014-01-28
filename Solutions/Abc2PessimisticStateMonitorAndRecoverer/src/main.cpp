#include "abc2pessimisticstatemonitorandrecoverer.h"

//TODOreq: at the very least I should put the "key prefixes" (not limited to prefixes, but i can't think of better word for it) into a common file as defines to be included by this app and abc2, but if I want to get all fancy n shit I can/should merge "core.json.docs" into that same file (so doc + source live together happily ever after). the existing paragraphs etc just become comments...
int main()
{
    Abc2PessimisticStateMonitorAndRecoverer abc2PessimisticStateMonitorAndRecoverer;
    return abc2PessimisticStateMonitorAndRecoverer.startPessimisticallyMonitoringAndRecovereringStateUntilToldToStop();
}

