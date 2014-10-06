#include "signalstatementnotchmultiplextergraphicsrect.h"

#include "designequalsimplementationguicommon.h"
#include "designequalsimplementationexistinsignalgraphicsitemforusecasescene.h"
#include "sourceexistingsignalsnappingindicationvisualrepresentation.h"

SignalStatementNotchMultiplexterGraphicsRect::SignalStatementNotchMultiplexterGraphicsRect(DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *parentSignalStatementGraphicsItem, const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent)
    , m_ParentSignalStatementGraphicsItem(parentSignalStatementGraphicsItem)
{ }
DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene *SignalStatementNotchMultiplexterGraphicsRect::parentSignalStatementGraphicsItem() const
{
    return m_ParentSignalStatementGraphicsItem;
}
int SignalStatementNotchMultiplexterGraphicsRect::type() const
{
    return DesignEqualsImplementationActorGraphicsItemForUseCaseScene_ExistingSignal_SignalStatementNotchMultiplexterGraphicsRect_GRAPHICS_TYPE_ID;
}
IRepresentSnapGraphicsItemAndProxyGraphicsItem* SignalStatementNotchMultiplexterGraphicsRect::makeSnappingHelperForMousePoint(QPointF eventScenePos)
{
    QPointF mouseItemPos = mapFromScene(eventScenePos);
    //QPointF mouseItemPos = mapFromScene(eventScenePos);
    //bool left = mouseItemPos.x() < 0;

    QMap<qreal /*distance*/, QPair<qreal /*vertical height*/, int /*index into vertical height array*/> > distancesFromMousePointAndTheirCorrespondingVerticalHeightsInOurInternalList_Sorter;
    int currentIndex = 0;
    Q_FOREACH(qreal currentVerticalPositionOfSnapPoint, m_ParentSignalStatementGraphicsItem->verticalPositionsOfSnapPoints())
    {
        distancesFromMousePointAndTheirCorrespondingVerticalHeightsInOurInternalList_Sorter.insert(qAbs(currentVerticalPositionOfSnapPoint - mouseItemPos.y()), qMakePair(currentVerticalPositionOfSnapPoint, currentIndex++));
    }
    if(!distancesFromMousePointAndTheirCorrespondingVerticalHeightsInOurInternalList_Sorter.isEmpty())
    {
        qreal closestSnappingPointsYValue = distancesFromMousePointAndTheirCorrespondingVerticalHeightsInOurInternalList_Sorter.first().first; //TODOreq: mouse pos is relative to signal statement (line), closestSnappingPointsYValue value is relative to the signal multiplexer rect..... yet i'm using the closestSnappingPointsYValue to setPos in the signal statement coordinates!

        IRepresentSnapGraphicsItemAndProxyGraphicsItem *sourceSnappingIndicationVisualRepresentation = new SourceExistingSignalSnappingIndicationVisualRepresentation(this, parentSignalStatementGraphicsItem()->indexStatementInsertedInto(), this, distancesFromMousePointAndTheirCorrespondingVerticalHeightsInOurInternalList_Sorter.first().second);
        //sourceSnappingIndicationVisualRepresentation->visualRepresentation()->setPos(QPointF((left ? line().p1().x() : line().p2().x()), closestSnappingPointsYValue));
        sourceSnappingIndicationVisualRepresentation->visualRepresentation()->setPos(QPointF(rect().right()+(DesignEqualsImplementationExistinSignalGraphicsItemForUseCaseScene_SLOT_CONNECTING_NOTCH_CIRCLE_RADIUS*2), closestSnappingPointsYValue));
        return sourceSnappingIndicationVisualRepresentation;
    }
    return 0;
}
int SignalStatementNotchMultiplexterGraphicsRect::getInsertSubIndexForMouseScenePos(QPointF eventScenePos)
{
    //TODOreq: this is LIKE snappig, but 'this' is known to be under the mouse. just like with slot statement inserting (getInsertIndexForMouseScenePos), some of this code is redundant with snapping and could be merged with it

    //some copy/pasta from makeSnappingHelperForMousePoint
    QPointF mouseItemPos = mapFromScene(eventScenePos);
    QMap<qreal, int> distancesFromMousePointAndTheirCorrespondingIndexesInOurInternalList_Sorter;
    Q_FOREACH(qreal currentVerticalPositionOfSnapPoint, parentSignalStatementGraphicsItem()->verticalPositionsOfSnapPoints())
    {
        distancesFromMousePointAndTheirCorrespondingIndexesInOurInternalList_Sorter.insert(qAbs(currentVerticalPositionOfSnapPoint - mouseItemPos.y()), currentVerticalPositionOfSnapPoint);
    }
    if(!distancesFromMousePointAndTheirCorrespondingIndexesInOurInternalList_Sorter.isEmpty())
    {
        return distancesFromMousePointAndTheirCorrespondingIndexesInOurInternalList_Sorter.first();
    }
    return 0;
}
