#include "clientwidget.h"

clientWidget::clientWidget(QWidget *parent)
    : QWidget(parent), m_Debug(0)
{
    QMetaObject::invokeMethod(this, "initGui", Qt::QueuedConnection);
}
clientWidget::~clientWidget()
{
    
}
void clientWidget::initGui()
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_AddSslClientButton = new QPushButton("Add SSL Client");

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_AddSslClientButton);
    this->setLayout(m_Layout);

    connect(m_AddSslClientButton, SIGNAL(clicked()), this, SIGNAL(addSslClientRequested()));
}
void clientWidget::handleD(const QString &msg)
{
    if(m_Debug)
    {
        m_Debug->appendPlainText(msg);
    }
}
