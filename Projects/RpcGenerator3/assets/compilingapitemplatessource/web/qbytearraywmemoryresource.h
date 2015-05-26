#include <Wt/WMemoryResource>
using namespace Wt;
#ifndef QBYTEARRAYWMEMORYRESOURCE_H
#define QBYTEARRAYWMEMORYRESOURCE_H

#include <QByteArray>

class QByteArrayWMemoryResource : public WMemoryResource
{
public:
    QByteArrayWMemoryResource(const QByteArray theByteArray, WObject *parent = 0);
    ~QByteArrayWMemoryResource();
private:
    QByteArray m_TheByteArray;
};

#endif // QBYTEARRAYWMEMORYRESOURCE_H
