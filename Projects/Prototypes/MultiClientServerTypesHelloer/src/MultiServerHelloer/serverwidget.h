#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLineEdit>

class ServerWidget : public QWidget
{
    Q_OBJECT    
public:
    ServerWidget(QWidget *parent = 0);
    ~ServerWidget();
private:
    QVBoxLayout *m_Layout;
    QPlainTextEdit *m_Debug;
    QPushButton *m_StartAll3Listening;
    QLineEdit *m_MessageLineEdit;
    void sendMessageToBusiness();
signals:
    void startAll3ListeningRequested();
    void messageSendRequested(const QString &message);
public slots:
    void initGui();
    void setReadyForBusinessMessageInteraction(bool ready);
    void handleD(const QString &msg);
};

#endif // SERVERWIDGET_H
