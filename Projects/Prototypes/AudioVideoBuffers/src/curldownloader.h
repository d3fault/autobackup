#ifndef CURLDOWNLOADER_H
#define CURLDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QTimer>

#include <localfile.h>

class CurlDownloader : public QObject
{
    Q_OBJECT
public:
    CurlDownloader(LocalFile *localFile);
private:
    QTimer *m_Timer;
    LocalFile *m_LocalFile;
signals:
    void onDataGathered(QByteArray *data);
public slots:
    void start();
private slots:
    void simulateReceivingData();
};

#endif // CURLDOWNLOADER_H
