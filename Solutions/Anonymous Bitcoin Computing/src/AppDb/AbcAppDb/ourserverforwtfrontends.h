#ifndef OURSERVERFORWTFRONTENDS_H
#define OURSERVERFORWTFRONTENDS_H

#include <QObject>

//this class maintains the connections to the wt front ends
//it does not matter from which wt front end an app logic request originates
//nor does it matter to which wt front end an app logic request response [primarily needs to go to].
//this class/thread is where the wt front-ends must 'race' (TODOreq: should _WE_ be the ones to catch the 2nd place entry (say, for puchasing a slot) and rejecting them... or should we just forward the event and let AppLogic do it? I guess it could be either but idk which is optimal and/or better design) to modify the AppDb (AppLogic)
//and this class also pushes all the AppLogic changes out to every connected Wt Front End (TODOop: we could prioritize the front end that actually made the request... for snappier responses etc.. since the others don't even necessarily (we can't know) care for the updates [yet]. this optimization is low priority, but actually sounds worthwhile)
class OurServerForWtFrontEnds : public QObject
{
    Q_OBJECT
public:
    explicit OurServerForWtFrontEnds(QObject *parent = 0);

signals:

public slots:

};

#endif // OURSERVERFORWTFRONTENDS_H
