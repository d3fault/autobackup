#ifndef SIMPLEWIDGET0_H
#define SIMPLEWIDGET0_H

#include <QStackedWidget>

class SimpleWidget0 : public QStackedWidget
{
    Q_OBJECT
public:
    SimpleWidget0(QWidget *parent = 0);
private:
    QWidget *pushButtonClickedWidget();

    QWidget *m_PushButtonClickedWidget;
private slots:
    void handlePushButtonClicked();
};

#endif // SIMPLEWIDGET0_H
