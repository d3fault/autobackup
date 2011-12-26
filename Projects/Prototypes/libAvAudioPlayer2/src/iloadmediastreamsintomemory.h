#ifndef ILOADMEDIASTREAMSINTOMEMORY_H
#define ILOADMEDIASTREAMSINTOMEMORY_H

#include <QObject>

class ILoadMediaStreamsIntoMemory : public QObject
{
    Q_OBJECT
public:
signals:
    void onDataGathered(QByteArray data);
    void d(const QString &);
public slots:
    virtual void init()=0;
    virtual void load()=0;
};

#endif // ILOADMEDIASTREAMSINTOMEMORY_H
