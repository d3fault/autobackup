#ifndef LOSSYDIRECTORYCOPIERGUI_H
#define LOSSYDIRECTORYCOPIERGUI_H

#include <QObject>
#include <QCoreApplication>

#include "objectonthreadhelper.h"
#include "lossydirectorycopier.h"
#include "lossydirectorycopierwidget.h"

class LossyDirectoryCopierGui : public QObject
{
    Q_OBJECT
public:
    explicit LossyDirectoryCopierGui(QObject *parent = 0);
private:
    ObjectOnThreadHelper<LossyDirectoryCopier> m_BusinessThread;
    LossyDirectoryCopierWidget m_Gui;
private slots:
    void handleLossyDirectoryCopierReadyForConnections();
    void handleAboutToQuit();
};

#endif // LOSSYDIRECTORYCOPIERGUI_H
