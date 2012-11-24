#include "serverwidget.h"

ServerWidget::ServerWidget(QWidget *parent)
    : QWidget(parent), m_Debug(0)
{
    QMetaObject::invokeMethod(this, "initGui", Qt::QueuedConnection);
}
ServerWidget::~ServerWidget()
{
    
}
void ServerWidget::sendMessageToBusiness()
{
    if(!m_MessageLineEdit->text().trimmed().isEmpty())
    {
        emit messageSendRequested(m_MessageLineEdit->text().trimmed());
    }
}
void ServerWidget::initGui()
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_StartAll3Listening = new QPushButton("Start All 3 Listening");
    m_MessageLineEdit = new QLineEdit();

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_StartAll3Listening);
    m_Layout->addWidget(m_MessageLineEdit);
    m_MessageLineEdit->setEnabled(false);
    m_MessageLineEdit->setText("boob");
    this->setLayout(m_Layout);

    connect(m_StartAll3Listening, SIGNAL(clicked()), this, SIGNAL(startAll3ListeningRequested()));
    connect(m_MessageLineEdit, SIGNAL(returnPressed()), this, SLOT(sendMessageToBusiness()));
}
void ServerWidget::setReadyForBusinessMessageInteraction(bool ready)
{
    m_MessageLineEdit->setEnabled(ready);
}
void ServerWidget::handleD(const QString &msg)
{
    if(m_Debug)
    {
        m_Debug->appendPlainText(msg);
    }
}
