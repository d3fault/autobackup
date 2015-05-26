#include "qbytearraywmemoryresource.h"

QByteArrayWMemoryResource::QByteArrayWMemoryResource(const QByteArray theByteArray, WObject *parent)
    : WMemoryResource(parent)
    , m_TheByteArray(theByteArray)
{
    setData((const unsigned char*)m_TheByteArray.constData(), m_TheByteArray.size());
}

QByteArrayWMemoryResource::~QByteArrayWMemoryResource()
{
    beingDeleted();
}
