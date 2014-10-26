#ifndef KEYPRESSINNEWEMPTYDOCTIMELINENODE_H
#define KEYPRESSINNEWEMPTYDOCTIMELINENODE_H

#include "../itimelinenode.h"
#include "../osioscommon.h"

class KeyPressInNewEmptyDocTimelineNodeData : public ITimelineNodeDataITimelineNodeData
{
public:
    KeyPressInNewEmptyDocTimelineNodeData(int keyPressed)
        : ITimelineNodeDataITimelineNodeData(TimelineNodeTypeEnum::KeyPressedInNewEmptyDocTimelineNode)
        , KeyPressed(keyPressed)
    { }
    KeyPressInNewEmptyDocTimelineNodeData(const KeyPressInNewEmptyDocTimelineNodeData &other)
        : ITimelineNodeDataITimelineNodeData(other)
        , KeyPressed(other.KeyPressed)
    { }
    virtual ~KeyPressInNewEmptyDocTimelineNodeData();

    int KeyPressed; //TODOoptional: multiple keys (modifiers). I really wish I could just serialize QKeyEvent :(

    virtual QDataStream &save(QDataStream &outputStream) const;
    virtual QDataStream &load(QDataStream &inputStream);
};

class KeyPressInNewEmptyDocTimelineNode : public ITimelineNode
{
  public:
    KeyPressInNewEmptyDocTimelineNode(int keyPressed)
        : ITimelineNode(TimelineNodeTypeEnum::KeyPressedInNewEmptyDocTimelineNode)
    {
        d = new KeyPressInNewEmptyDocTimelineNodeData(keyPressed);
    }
    KeyPressInNewEmptyDocTimelineNode(const KeyPressInNewEmptyDocTimelineNode &other)
        : ITimelineNode(other)
        , d(other.d)
    { }
    virtual ~KeyPressInNewEmptyDocTimelineNode();

    void setKeyPressed(int keyPressed) { d->KeyPressed = keyPressed; }
    int keyPressed() const { return d->KeyPressed; }
  private:
    QSharedDataPointer<KeyPressInNewEmptyDocTimelineNodeData> d;
};

#endif // KEYPRESSINNEWEMPTYDOCTIMELINENODE_H
