#include "keypressinnewemptydoctimelinenode.h"

#include <QKeyEvent>

KeyPressInNewEmptyDocTimelineNodeData::~KeyPressInNewEmptyDocTimelineNodeData()
{ }
#if 0
QString KeyPressInNewEmptyDocTimelineNodeData::humanReadableShortDescription() const
{
    QKeyEvent theKeyAsEvent(QKeyEvent::KeyPress, KeyPressed, Qt::NoModifier); //used convert from int to string
    return "Key Press Event (" + theKeyAsEvent.text() + ")";
}
#endif
QDataStream &KeyPressInNewEmptyDocTimelineNodeData::save(QDataStream &outputStream) const
{
    ITimelineNodeDataITimelineNodeData::save(outputStream);
    outputStream << KeyPressed;
    return outputStream;
}
QDataStream &KeyPressInNewEmptyDocTimelineNodeData::load(QDataStream &inputStream)
{
    ITimelineNodeDataITimelineNodeData::load(inputStream);
    inputStream >> KeyPressed;
    return inputStream;
}
KeyPressInNewEmptyDocTimelineNode::~KeyPressInNewEmptyDocTimelineNode()
{ }
