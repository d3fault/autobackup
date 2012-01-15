#ifndef CURLDOWNLOADER_H
#define CURLDOWNLOADER_H

#include <QObject>
#include <QByteArray>

class CurlDownloader : public QObject
{
    Q_OBJECT
public:
    explicit CurlDownloader(QObject *parent = 0);

signals:
    void onDataGathered(QByteArray *data);

public slots:

};

#endif // CURLDOWNLOADER_H
