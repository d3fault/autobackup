#include "osioswebbrowserwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QtWebKitWidgets/QWebView>
#include <QWebSettings>

//TODOreq: history and saved files are all persisted forever (TODOoptional: incognito/etc modes)
//TODOreq-MAYBE-idk-since-easy-might-as-well-but-ultimately-https-is-a-piece-of-shit[because-of-key-distribution]: green thing if https validates
//TODOoptional: noscript per-site whitelists type shit (should resemble more abe than generic noscript imo)
OsiosWebBrowserWidget::OsiosWebBrowserWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    //TODOreq: tons of other features, back, home, bookmarks, cookie management, etc

    QHBoxLayout *urlRow = new QHBoxLayout();
    m_UrlLineEdit = new QLineEdit(); //TODOreq: when single clicked for first time, THAT text vanishes
    m_UrlLineEdit->setPlaceholderText(tr("Type a URL here..."));
    //TODOoptional: when implementing "back", think about if/how it can integrate directly with timeline view. Then again, the action of "going back" is itself an action FORWARD on timeline view, so maybe don't integrate at all (aside from mentioning in timeline view that back button was clicked as additional data). Related, and off-topic simultaneously: in the simple "notepad" editor, the "undo" action is also a "forward" action in timeline view. Everything is just a mutation
    QPushButton *urlGoButton = new QPushButton(tr("Go"));

    m_WebView = new QWebView();
    m_WebView->settings()->setAttribute(QWebSettings::JavascriptEnabled, false); //TODOreq: on first run explain and ask "secure vs insecure" mode. space-bar/enter (yes junkies) = "secure" mode (javascript disabled)

    urlRow->addWidget(m_UrlLineEdit);
    urlRow->addWidget(urlGoButton);

    myLayout->addLayout(urlRow);
    myLayout->addWidget(m_WebView, 1);

    setLayout(myLayout);

    connect(m_UrlLineEdit, SIGNAL(returnPressed()), this, SLOT(browseToUrlLineEditContents()));
    connect(urlGoButton, SIGNAL(clicked()), this, SLOT(browseToUrlLineEditContents()));
}
void OsiosWebBrowserWidget::browseToUrlLineEditContents()
{
    m_WebView->setUrl(QUrl::fromUserInput(m_UrlLineEdit->text()));
}
