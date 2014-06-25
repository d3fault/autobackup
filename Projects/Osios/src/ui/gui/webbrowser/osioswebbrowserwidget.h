#ifndef OSIOSWEBBROWSERWIDGET_H
#define OSIOSWEBBROWSERWIDGET_H

#include <QWidget>

class QLineEdit;
class QWebView;

class OsiosWebBrowserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OsiosWebBrowserWidget(QWidget *parent = 0);
private:
    QLineEdit *m_UrlLineEdit;
    QWebView *m_WebView;
private slots:
    void browseToUrlLineEditContents();
};

#endif // OSIOSWEBBROWSERWIDGET_H
