#ifndef BULKFILETEXTPREPENDERGUI_H
#define BULKFILETEXTPREPENDERGUI_H

#include <QObject>
#include <QCoreApplication>

#include "../lib/bulkfiletextprepender.h"
#include "bulkfiletextprependerwidget.h"
#include "objectonthreadhelper.h"

class BulkFileTextPrependerGui : public QObject
{
    Q_OBJECT
public:
    explicit BulkFileTextPrependerGui(QObject *parent = 0);
private:
    ObjectOnThreadHelper<BulkFileTextPrepender> m_BusinessThread;
    BulkFileTextPrependerWidget m_Gui;
signals:

public slots:
    void handleBulkFileTextPrependerReadyForConnections();
    void handleAboutToQuit();
};

#endif // BULKFILETEXTPREPENDERGUI_H
