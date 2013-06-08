#ifndef BANKSERVERCLIENTDEBUGWIDGET_H
#define BANKSERVERCLIENTDEBUGWIDGET_H

#include <QtGui/QWidget>
#include <QPushButton>
#include <QLineEdit>

class BankServerClientDebugWidget : public QWidget
{
    Q_OBJECT
public:
    BankServerClientDebugWidget(QWidget *parent = 0);
    void addButtonsToLayoutAndConnectToBackend();
private:
    QLineEdit *m_UsernameLineEdit;
    QPushButton *m_SimulateCreateBankAccountButton;
    QPushButton *m_SimulateGetAddFundsKeyButton;
signals:
    void startBusinessRequested();
    void simulateCreateBankAccountActionRequested(const QString &username);
    void simulateGetAddFundsKeyActionRequested(const QString &username);
    void stopBusinessRequested();
private slots:
    void handleSimulateCreateBankAccountClicked();
    void handleSimulateGetAddFundsKeyClicked();
public slots:
    void handleBusinessInitialized();
    void handleBusinessStarted();
    void handleBusinessStopped();
};

#endif // BANKSERVERCLIENTDEBUGWIDGET_H
