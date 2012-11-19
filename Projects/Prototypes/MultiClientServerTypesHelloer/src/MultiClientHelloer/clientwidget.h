#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QtGui/QWidget>

class clientWidget : public QWidget
{
    Q_OBJECT
public:
    clientWidget(QWidget *parent = 0);
    ~clientWidget();
public slots:
    void handleD(const QString &msg);
};

#endif // CLIENTWIDGET_H
