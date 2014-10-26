#include "keypressinnewemptydoctimelinenode.h"

#include <QKeySequence>

#if 0
KeyPressInNewEmptyDocTimelineNodeData::~KeyPressInNewEmptyDocTimelineNodeData()
{ }
#endif
KeyPressInNewEmptyDocTimelineNode::~KeyPressInNewEmptyDocTimelineNode()
{ }
QString KeyPressInNewEmptyDocTimelineNode::humanReadableShortDescription() const
{
    return "Key Press Event (" + QKeySequence(KeyPressed).toString() + ")";
}
QDataStream &KeyPressInNewEmptyDocTimelineNode::save(QDataStream &outputStream) const
{
    ITimelineNode::save(outputStream);
    outputStream << KeyPressed;
    return outputStream;
}
QDataStream &KeyPressInNewEmptyDocTimelineNode::load(QDataStream &inputStream)
{
    ITimelineNode::load(inputStream);
    inputStream >> KeyPressed;
    return inputStream;
}
