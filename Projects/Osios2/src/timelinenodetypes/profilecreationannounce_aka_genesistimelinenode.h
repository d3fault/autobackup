#ifndef PROFILECREATIONANNOUNCE_AKA_GENESISTIMELINENODE_H
#define PROFILECREATIONANNOUNCE_AKA_GENESISTIMELINENODE_H

#include "../itimelinenode.h"

#include <QString>

class ProfileCreationAnnounce_aka_GenesisTimelineNode : public ITimelineNode
{
public:
    ProfileCreationAnnounce_aka_GenesisTimelineNode()
        : ITimelineNode(TimelineNodeTypeEnum::ProfileCreationAnnounce_aka_GenesisTimelineNode, 0)
    { }
    ProfileCreationAnnounce_aka_GenesisTimelineNode(QString profileName, qint64 unixTimestamp_OrZeroToUseCurrentTime = 0)
        : ITimelineNode(TimelineNodeTypeEnum::ProfileCreationAnnounce_aka_GenesisTimelineNode, unixTimestamp_OrZeroToUseCurrentTime)
        , ProfileName(profileName)
    { }
    ProfileCreationAnnounce_aka_GenesisTimelineNode(const ProfileCreationAnnounce_aka_GenesisTimelineNode &other)
        : ITimelineNode(other)
        , ProfileName(other.ProfileName)
    { }
    virtual ~ProfileCreationAnnounce_aka_GenesisTimelineNode();

    QString ProfileName; //kind of redundant since every timeline node on network has profile name in it. fuck it for now KISS/understandability >

    virtual QString humanReadableShortDescription() const;
protected:
    virtual QDataStream &save(QDataStream &outputStream) const;
    virtual QDataStream &load(QDataStream &inputStream);
};

#endif // PROFILECREATIONANNOUNCE_AKA_GENESISTIMELINENODE_H
