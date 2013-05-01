#ifndef EASYTREEGUI_H
#define EASYTREEGUI_H

#include <QObject>
#include <QCoreApplication>

#include "objectonthreadhelper.h"
#include "easytreeguibusiness.h"
#include "easytreeguiwidget.h"

class EasyTreeGui : public QObject
{
    Q_OBJECT
public:
    explicit EasyTreeGui(QObject *parent = 0);
private:
    ObjectOnThreadHelper<EasyTreeGuiBusiness> m_EasyTreeGuiBusinessThreadHelper;
    EasyTreeGuiWidget m_EasyTreeGuiWidget;
private slots:
    void handleEasyTreeGuiBusinessInstantiated();
    void handleAboutToQuit();
};

#endif // EASYTREEGUI_H
