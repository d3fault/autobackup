#ifndef PARCELIZEDDEFINITION_H
#define PARCELIZEDDEFINITION_H

#include <QtCore/QDataStream>

struct Message
{
    enum MessageType
    {
        InvalidMessage,
        ParcelizedDefinitionMessage
    };

    quint32 version;
    quint16 messageType;

};

struct ParcelizedDefinition : Message
{

};

#endif // PARCELIZEDDEFINITION_H
