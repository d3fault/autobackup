#ifndef QFSW2TESTWIDGET_H
#define QFSW2TESTWIDGET_H

#include <QtGui/QWidget>

#include "../../../../GloballySharedClasses/idebuggablestartablestoppablefrontend.h"

class QFsw2TestWidget : public IDebuggableStartableStoppableFrontend
{
    Q_OBJECT
public:
    QFsw2TestWidget(QWidget *parent = 0);
    ~QFsw2TestWidget();
};

#endif // QFSW2TESTWIDGET_H
