#ifndef MULTISERVERHELLOERTEST_H
#define MULTISERVERHELLOERTEST_H

#include <QObject>

#include "serverwidget.h"
#include "multiserverbusiness.h"
#include "objectonthreadhelper.h"

class MultiServerHelloerTest : public QObject
{
    Q_OBJECT
public:
    explicit MultiServerHelloerTest(QObject *parent = 0);
private:
    ServerWidget m_Gui;
    ObjectOnThreadHelper<MultiServerBusiness> m_MultiServerBusinessThreadHelper;
public slots:
    void multiServerHelloerReadyForConnections();
};

#endif // MULTISERVERHELLOERTEST_H
