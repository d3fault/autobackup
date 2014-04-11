#include "runtimedynamiciterationsdeterminerretainingresponsiveness.h"

#include <QCryptographicHash>

RuntimeDynamicIterationsDeterminerRetainingResponsiveness::RuntimeDynamicIterationsDeterminerRetainingResponsiveness(QObject *parent) :
    QObject(parent), m_DeterminedIterationsToRetainResponsiveness(0)
{ }
//more advanced designs could use a pure virtual here :-P
void RuntimeDynamicIterationsDeterminerRetainingResponsiveness::doSingleIterationOfWork()
{
    //bullshit busy work
    static qint64 x = 0;
    static QByteArray hashResult;
    ++x;
    hashResult = QCryptographicHash::hash(QString::number(x).toUtf8(), QCryptographicHash::Sha1);
    if(x % 100000 == 0)
        emit d(hashResult);
}
//example results, core 2 quad 2.83ghz in a VM:
	//we determined that we can do 196418 sha1 iterations while still being responsive within 250 milliseconds
	//Ping -> Pong took 293 milliseconds. Is that less than what's in the above line edit :-D? <-- my input was 250. It makes sense why it's off, the more realistic max is n*2. I could account for that in the code, but too lazy for now. I'm happy with it as is and I'm just dicking around anyways :-P
void RuntimeDynamicIterationsDeterminerRetainingResponsiveness::determineIterationsToRetainResponsivenessWithin(qint64 msecTargetMaxResponseTime)
{
    //it would be more accurate to just increment by 1 instead of using golden ratio, but that would incur a greater startup time. further analysis could be done once msecTargetMaxResponseTime is surpased, ie a hybrid golden ratio and "increment by 1" solution. for now, KISS
    //use golden ratio increasing amount of iterations, but don't start at 1
    qint64 previousIterationsCount = 3; //2 + _3_ = 5
    qint64 currentIterationsCount = 5; //1 probably too low/stupid to benchmark, might change this later

    qint64 currentElapsedTime = 0;
    while(currentElapsedTime < msecTargetMaxResponseTime)
    {
        m_BenchmarkTimer.start();
        for(qint64 i = 0; i < currentIterationsCount; ++i)
        {
            doSingleIterationOfWork();
        }
        currentElapsedTime = m_BenchmarkTimer.elapsed();

        //use golden ratio increasing amount of iterations
        qint64 temp = currentIterationsCount;
        currentIterationsCount = previousIterationsCount + currentIterationsCount;
        previousIterationsCount = temp;
    }
    m_DeterminedIterationsToRetainResponsiveness = previousIterationsCount; //current would be > our target, whereas previous is just under it ;-P
    emit d("we determined that we can do " + QString::number(m_DeterminedIterationsToRetainResponsiveness) + " sha1 iterations while still being responsive within " + QString::number(msecTargetMaxResponseTime) + " milliseconds");
}
void RuntimeDynamicIterationsDeterminerRetainingResponsiveness::startDoingWorkUtilizingDeterminedNumberOfIterations()
{
    if(m_DeterminedIterationsToRetainResponsiveness == 0)
    {
        emit d("you need to determine the number of iterations first"); //yes yes, they could be daisy chained and the determininig done once transparently at startup...
        return;
    }
    m_StopRequested = false;
    doMultipleUnitsOfWorkUsingDeterminedIterations();
}
//Dear Vin Diesel, here is an example of one unit of execution (and in this case, we really are trying to maximize speeeeeeeeed xD):
void RuntimeDynamicIterationsDeterminerRetainingResponsiveness::doMultipleUnitsOfWorkUsingDeterminedIterations()
{
    if(m_StopRequested)
    {
        emit d("backend: stopped");
        return;
    }
    for(qint64 i = 0; i < m_DeterminedIterationsToRetainResponsiveness; ++i)
    {
        doSingleIterationOfWork();
    }
    QMetaObject::invokeMethod(this, "doMultipleUnitsOfWorkUsingDeterminedIterations", Qt::QueuedConnection); //queued connection vital here. TODOoptimization: find out if it's faster to just call processEvents() instead (in which case we just enclose this entire method body in a forever loop). still i like this queued invoke better DESIGN wise
}
void RuntimeDynamicIterationsDeterminerRetainingResponsiveness::ping()
{
    emit pong();
}
void RuntimeDynamicIterationsDeterminerRetainingResponsiveness::stopDoingWork()
{
    emit d("backend: stopping...");
    m_StopRequested = true;
}
