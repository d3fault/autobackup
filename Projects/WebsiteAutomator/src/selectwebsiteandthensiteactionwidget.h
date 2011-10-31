#ifndef SELECTWEBSITEANDTHENSITEACTIONWIDGET_H
#define SELECTWEBSITEANDTHENSITEACTIONWIDGET_H

#include <QtGui/QWidget>

class QVBoxLayout;
class QPushButton;
class QList;
class WebsiteAutomator;

class selectWebsiteAndThenSiteActionWidget : public QWidget
{
    Q_OBJECT

public:
    selectWebsiteAndThenSiteActionWidget(QWidget *parent = 0);
    ~selectWebsiteAndThenSiteActionWidget();
private:
    QVBoxLayout *m_Layout;
    QPushButton *m_GoButton;
    QList<WebsiteAutomator*> *m_WebsiteAutomatorList;
private slots:
    void handleGoButtonClicked();
};

#endif // SELECTWEBSITEANDTHENSITEACTIONWIDGET_H
