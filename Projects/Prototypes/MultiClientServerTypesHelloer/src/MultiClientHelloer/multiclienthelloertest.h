#ifndef MULTICLIENTHELLOERTEST_H
#define MULTICLIENTHELLOERTEST_H

#include <QObject>
#include <QCoreApplication>

#include "clientwidget.h"
#include "objectonthreadhelper.h"
#include "multiclientbusiness.h"

class MultiClientHelloerTest : public QObject
{
    Q_OBJECT
public:
    explicit MultiClientHelloerTest(QObject *parent = 0);
    ~MultiClientHelloerTest();
private:
    clientWidget m_Gui;
    ObjectOnThreadHelper<MultiClientBusiness> m_MultiClientBusinessThreadHelper;

    bool m_CleanedUpBackends;
private slots:
    void multiClientHelloerReadyForConnections();
    void cleanupBackendObjectsIfNeeded();
};

#endif // MULTICLIENTHELLOERTEST_H
