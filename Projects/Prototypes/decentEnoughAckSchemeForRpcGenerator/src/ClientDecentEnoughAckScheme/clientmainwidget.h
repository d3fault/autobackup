#ifndef CLIENTMAINWIDGET_H
#define CLIENTMAINWIDGET_H

#include <QtGui/QWidget>

class ClientMainWidget : public QWidget
{
    Q_OBJECT    
public:
    ClientMainWidget(QWidget *parent = 0);
    ~ClientMainWidget();
signals:
    void startClientBackendRequested();
    void stopClientBackendRequested();
public slots:
    void handleD(const QString &msg);
};

#endif // CLIENTMAINWIDGET_H
