#ifndef MULTISERVERHELLOERTEST_H
#define MULTISERVERHELLOERTEST_H

#include <QObject>
#include <QCoreApplication>

#include "serverwidget.h"
#include "multiserverbusiness.h"
#include "objectonthreadhelper.h"

class MultiServerHelloerTest : public QObject
{
    Q_OBJECT
public:
    explicit MultiServerHelloerTest(QObject *parent = 0);
    ~MultiServerHelloerTest();
private:
    ServerWidget m_Gui;
    ObjectOnThreadHelper<MultiServerBusiness> m_MultiServerBusinessThreadHelper;

    void cleanupBackendObjectsIfNeeded();
    bool m_CleanedUpBackends;
public slots:
    void multiServerHelloerReadyForConnections();
private slots:
    void handleAboutToQuit();
};

#endif // MULTISERVERHELLOERTEST_H
