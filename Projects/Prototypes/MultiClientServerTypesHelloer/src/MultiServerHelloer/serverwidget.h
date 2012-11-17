#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>

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
signals:
    void startAll3ListeningRequested();
public slots:
    void initGui();
    void handleD(const QString &msg);
};

#endif // SERVERWIDGET_H
