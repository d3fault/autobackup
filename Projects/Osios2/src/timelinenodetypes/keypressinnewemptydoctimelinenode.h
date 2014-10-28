#ifndef KEYPRESSINNEWEMPTYDOCTIMELINENODE_H
#define KEYPRESSINNEWEMPTYDOCTIMELINENODE_H

#include "../itimelinenode.h"

class KeyPressInNewEmptyDocTimelineNode : public ITimelineNode
{
  public:
    KeyPressInNewEmptyDocTimelineNode()
        : ITimelineNode(TimelineNodeTypeEnum::KeyPressedInNewEmptyDocTimelineNode, 0)
        , KeyPressed(-1)
    { }
    KeyPressInNewEmptyDocTimelineNode(int keyPressed, qint64 unixTimestamp_OrZeroToUseCurrentTime = 0)
        : ITimelineNode(TimelineNodeTypeEnum::KeyPressedInNewEmptyDocTimelineNode, unixTimestamp_OrZeroToUseCurrentTime)
        , KeyPressed(keyPressed)
    { }
    KeyPressInNewEmptyDocTimelineNode(const KeyPressInNewEmptyDocTimelineNode &other)
        : ITimelineNode(other)
        , KeyPressed(other.KeyPressed)
    { }
    virtual ~KeyPressInNewEmptyDocTimelineNode();

    int KeyPressed;

    virtual QString humanReadableShortDescription() const;
protected:
    virtual QDataStream &save(QDataStream &outputStream) const;
    virtual QDataStream &load(QDataStream &inputStream);
};

#if 0
#include "../osioscommon.h"

class KeyPressInNewEmptyDocTimelineNodeData : public ITimelineNodeData
{
public:
    KeyPressInNewEmptyDocTimelineNodeData(int keyPressed)
        : ITimelineNodeData(TimelineNodeTypeEnum::KeyPressedInNewEmptyDocTimelineNode)
        , KeyPressed(keyPressed)
    { }
    KeyPressInNewEmptyDocTimelineNodeData(const KeyPressInNewEmptyDocTimelineNodeData &other)
        : ITimelineNodeData(other)
        , KeyPressed(other.KeyPressed)
    { }
    virtual ~KeyPressInNewEmptyDocTimelineNodeData();

    int KeyPressed; //TODOoptional: multiple keys (modifiers)

    virtual QDataStream &save(QDataStream &outputStream) const;
    virtual QDataStream &load(QDataStream &inputStream);
};

class KeyPressInNewEmptyDocTimelineNode : public ITimelineNode<KeyPressInNewEmptyDocTimelineNodeData>
{
  public:
    KeyPressInNewEmptyDocTimelineNode(int keyPressed)
        : ITimelineNode(TimelineNodeTypeEnum::KeyPressedInNewEmptyDocTimelineNode)
    {
        d = new KeyPressInNewEmptyDocTimelineNodeData(keyPressed);
    }
    KeyPressInNewEmptyDocTimelineNode(const KeyPressInNewEmptyDocTimelineNode &other)
        : ITimelineNode<KeyPressInNewEmptyDocTimelineNodeData>(other)
        , d(other.d)
    { }
    virtual ~KeyPressInNewEmptyDocTimelineNode();

    void setKeyPressed(int keyPressed) { d->KeyPressed = keyPressed; }
    int keyPressed() const { return d->KeyPressed; }
  private:
    QSharedDataPointer<KeyPressInNewEmptyDocTimelineNodeData> d;
};
#endif

#endif // KEYPRESSINNEWEMPTYDOCTIMELINENODE_H
