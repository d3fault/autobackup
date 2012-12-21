#ifndef BANKSERVERDEBUGWIDGET_H
#define BANKSERVERDEBUGWIDGET_H

#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>

class bankServerDebugWidget : public QWidget
{
    Q_OBJECT
public:
    bankServerDebugWidget(QWidget *parent = 0);
private:
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QPushButton *m_StartBusinessButton;
    QPushButton *m_SimulatePendingBalanceDetectedBroadcastButton;
    QPushButton *m_SimulateConfirmedBalanceDetectedBroadcastButton;
    QPushButton *m_StopBusinessButton;
    void setUserInteractionEnabled(bool enabled);
    bool m_BusinessStarted;
signals:
    void startBusinessRequested();
    void simulatePendingBalanceDetectedBroadcastRequested();
    void simulateConfirmedBalanceDetectedBroadcastRequested();
    void stopBusinessRequested();
private slots:
    void disableUserInteraction() { setUserInteractionEnabled(false); }
    void enableUserInteraction() { setUserInteractionEnabled(true); }
public slots:
    void handleD(const QString &msg);
    void handleBusinessInitialized();
    void handleBusinessStarted();
    void handleBusinessStopped();
};

#endif // BANKSERVERDEBUGWIDGET_H
