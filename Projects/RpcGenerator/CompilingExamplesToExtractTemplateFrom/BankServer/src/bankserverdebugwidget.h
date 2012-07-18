#ifndef BANKSERVERDEBUGWIDGET_H
#define BANKSERVERDEBUGWIDGET_H

#include <QtGui/QWidget>

class bankServerDebugWidget : public QWidget

{
    Q_OBJECT

public:
    bankServerDebugWidget(IDebuggableStartableStoppableBankServerBackend *debuggableStartableStoppableBankServerBackend);
    ~bankServerDebugWidget();
private:
    IDebuggableStartableStoppableBankServerBackend* m_DebuggableStartableStoppableBankServerBackend;
};

#endif // BANKSERVERDEBUGWIDGET_H
