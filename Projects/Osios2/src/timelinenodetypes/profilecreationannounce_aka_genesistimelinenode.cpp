#include "profilecreationannounce_aka_genesistimelinenode.h"

#include <QObject>

ProfileCreationAnnounce_aka_GenesisTimelineNode::~ProfileCreationAnnounce_aka_GenesisTimelineNode()
{ }
QString ProfileCreationAnnounce_aka_GenesisTimelineNode::humanReadableShortDescription() const
{
    return QObject::tr("Profile Created: ") + ProfileName;
}
QDataStream &ProfileCreationAnnounce_aka_GenesisTimelineNode::save(QDataStream &outputStream) const
{
    outputStream << ProfileName;
    return outputStream;
}
QDataStream &ProfileCreationAnnounce_aka_GenesisTimelineNode::load(QDataStream &inputStream)
{
    inputStream >> ProfileName;
    return inputStream;
}
