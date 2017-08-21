#include "simplewidget0.h"

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

SimpleWidget0::SimpleWidget0(QWidget *parent)
    : QStackedWidget(parent)
    , m_PushButtonClickedWidget(nullptr)
{
    QWidget myWidget = new QWidget(this);
    QVBoxLayout *myLayout = new QVBoxLayout(myWidget);
    QLabel *myHelpLink = new QLabel(tr("Help"), myLayout);
    QPushButton *myPushButton = new QPushButton();
    myLayout->addWidget(myHelpLink);
    myLayout->addWidget(myPushButton);
    setCurrentWidget(myWidget);

    connect(myHelpLink , &QLabel::linkActivated, this, &SimpleWidget0::handleHelpLinkActivated);
    connect(myPushButton, &QPushButton::clicked, this, &SimpleWidget0::handlePushButtonClicked);
}
QWidget *SimpleWidget0::pushButtonClickedWidget()
{
    if(m_PushButtonClickedWidget == nullptr)
    {
        m_PushButtonClickedWidget = new QWidget(this);
        QVBoxLayout *myLayout = new QVBoxLayout(m_PushButtonClickedWidget);
        QLabel *myLabel = new QLabel(tr("You have come to the end of the program"));
        myLayout->addWidget(myLabel, 1, Qt::AlignCenter);
    }
    return m_PushButtonClickedWidget;
}
void SimpleWidget0::handlePushButtonClicked()
{
    setCurrentWidget(pushButtonClickedWidget());
}
