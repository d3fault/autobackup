#ifndef CONCURRENTLOSSYDIRECTORYCOPIERGUI_H
#define CONCURRENTLOSSYDIRECTORYCOPIERGUI_H

#include <QObject>
#include <QCoreApplication>

#include "objectonthreadhelper.h"
#include "concurrentlossydirectorycopier.h"
#include "concurrentlossydirectorycopierwidget.h"

class ConcurrentLossyDirectoryCopierGui : public QObject
{
    Q_OBJECT
public:
    explicit ConcurrentLossyDirectoryCopierGui(QObject *parent = 0);
private:
    ObjectOnThreadHelper<ConcurrentLossyDirectoryCopier> m_BusinessThread;
    ConcurrentLossyDirectoryCopierWidget m_Gui;
public slots:
    void handleConcurrentLossyDirectoryCopierReadyForConnections();
    void handleAboutToQuit();
};

#endif // CONCURRENTLOSSYDIRECTORYCOPIERGUI_H
