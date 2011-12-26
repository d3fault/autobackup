#ifndef ILOADMEDIASTREAMSINTOMEMORY_H
#define ILOADMEDIASTREAMSINTOMEMORY_H

#include <QObject>

class ILoadMediaStreamsIntoMemory : public QObject
{
    Q_OBJECT
public:
signals:
    void onDataGathered(QByteArray data);
public slots:
    virtual void init()=0;
    virtual void loadLoop()=0;
};

#endif // ILOADMEDIASTREAMSINTOMEMORY_H
