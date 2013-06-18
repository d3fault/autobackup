#ifndef LIFESHAPERSTATICFILESTEST_H
#define LIFESHAPERSTATICFILESTEST_H

#include <QObject>
#include <QCoreApplication>

#include "lifeshaperstaticfileswidget.h"
#include "lifeshaperstaticfilesbusiness.h"
#include "objectonthreadhelper.h"

class LifeShaperStaticFilesTest : public QObject
{
    Q_OBJECT
public:
    explicit LifeShaperStaticFilesTest(QObject *parent = 0);
private:
    LifeShaperStaticFilesWidget m_Gui;
    ObjectOnThreadHelper<LifeShaperStaticFilesBusiness> m_BusinessThread;
public slots:
    void handleLifeShaperStaticFilesBusinessInstantiated();
    void handleAboutToQuit();
};

#endif // LIFESHAPERSTATICFILESTEST_H
