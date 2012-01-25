#ifndef CURLDOWNLOADER_H
#define CURLDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QTimer>

#include <generateddatabuffer.h>
#include <databuffergenerator.h>

class CurlDownloader : public QObject
{
    Q_OBJECT
public:
    CurlDownloader(DataBufferGenerator *dataBufferGenerator);
private:
    QTimer *m_Timer;
    DataBufferGenerator *m_DataBufferGeneator;
signals:
    void onDataGathered(QByteArray *data);
public slots:
    void start();
private slots:
    void simulateReceivingData();
};

#endif // CURLDOWNLOADER_H
