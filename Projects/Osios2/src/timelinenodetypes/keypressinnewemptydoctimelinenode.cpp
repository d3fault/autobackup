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
    return QObject::tr("Key Press Event (") + QKeySequence(KeyPressed).toString() + ")";
}
QDataStream &KeyPressInNewEmptyDocTimelineNode::save(QDataStream &outputStream) const
{
    outputStream << KeyPressed;
    return outputStream;
}
QDataStream &KeyPressInNewEmptyDocTimelineNode::load(QDataStream &inputStream)
{
    inputStream >> KeyPressed;
    return inputStream;
}
