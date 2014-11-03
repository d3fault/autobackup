#ifndef IOSIOSCLIENT_H
#define IOSIOSCLIENT_H

#include <QByteArray>

#include "itimelinenode.h"

class QObject;
class QWidget;

class OsiosDhtPeer;

class IOsiosClient
{
public:
    static QByteArray calculateCryptographicHashOfWidgetRenderedToImage(QWidget *widgetToRenderAndCalculateSha1SumOfRenderingFor);

    //QByteArray synthesizeTimelineNodeAndReturnSha1OfImageRenderingToProveIt(TimelineNode action)=0;
    virtual QObject *asQObject()=0;
protected: //signals
    //RECORD ACTUAL EVENT LOCALLY
    virtual void actionOccurred(TimelineNode action, const QByteArray &cryptographicHashOfTheRenderingOfGuiAfterTheActionWasApplied)=0;

    //SYNTHESIZE REPLICAS EVENTS REMOTELY
    virtual void timelineNodeAppliedAndRendered(OsiosDhtPeer *osiosDhtPeer, TimelineNode action, const QByteArray &renderingProof_Sha1OfPngRofl)=0;
};

#endif // IOSIOSCLIENT_H
