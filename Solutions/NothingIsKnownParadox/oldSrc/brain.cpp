#include "brain.h"

Brain::Brain(QObject *parent) :
    QObject(parent)
{
    connect(this, SIGNAL(reportWhetherOrNotNothingIsKnown(bool)), this, SLOT(addReportResultsToListOfThingsKnown(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(reEvaluateAllThingsKnownRequested()), this, SLOT(reEvaluateAllThingsKnown()), Qt::QueuedConnection);
    connect(this, SIGNAL(removeFromThingsKnownRequested(QList<QString>)), this, SLOT(removeFromThingsKnown(QList<QString>)), Qt::QueuedConnection);
}
bool Brain::isTrue(QString claim)
{
    switch(claim)
    {
    case THE_CLAIM:
    {
        if(m_ThingsKnown.size() < 1)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    break;
    case THE_OPPOSITE_CLAIM:
    {
        if(m_ThingsKnown.size() > 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    break;
    default:
        return false;
    break;
    }
    return false;
}
void Brain::determineWhetherOrNotNothingIsKnown()
{
    emit reportWhetherOrNotNothingIsKnown(m_ThingsKnown.size() < 1);
}
void Brain::addReportResultsToListOfThingsKnown(bool nothingIsKnown)
{
    if(nothingIsKnown)
    {
        m_ThingsKnown.append(THE_CLAIM);
    }
    else
    {
        m_ThingsKnown.append(THE_OPPOSITE_CLAIM); //does this fuck shit up? it's certainly implied, but i think it might mess up my infinite toggling :(
        //hmm analyzing "how" this "something is known" fits and how it breaks the toggling effect (it always stays in m_ThingsKnown and can not be removed, it depends on itself (or something)!), appears, to my mathematically unworthy mind, to be a mathematical proof of something being known
    }

    //the heart of the paradox, the source of the infinite toggling
    emit reEvaluateAllThingsKnownRequested();
}
void Brain::reEvaluateAllThingsKnown()
{
    QMutableListIterator<QString> it(m_ThingsKnown);
    while(it.hasNext())
    {
        QString current = it.next();
        if(!isTrue(current))
        {
            it.remove();
            //TODOreq: does a re-evaluate go here too (a boolean flag if ANY have been removed)? It makes sense that yes it would get re-evaluated on removal, and it also exposes some weird error cases where inter dependent things known are determined true or false based on their position in the list! but for such a small example such as this, that error case will probably never occur
        }
    }

    //as a sort of special hack for this app, at the end of every re-evaluation we try to determine whether or not nothing is known
    determineWhetherOrNotNothingIsKnown();
}
