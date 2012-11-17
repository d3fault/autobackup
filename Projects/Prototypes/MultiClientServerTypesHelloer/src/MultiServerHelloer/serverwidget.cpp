#include "serverwidget.h"

ServerWidget::ServerWidget(QWidget *parent)
    : QWidget(parent), m_Debug(0)
{
    QMetaObject::invokeMethod(this, "initGui", Qt::QueuedConnection);
}
ServerWidget::~ServerWidget()
{
    
}
void ServerWidget::initGui()
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_StartAll3Listening = new QPushButton("Start All 3 Listening");

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_StartAll3Listening);
    this->setLayout(m_Layout);

    connect(m_StartAll3Listening, SIGNAL(clicked()), this, SIGNAL(startAll3ListeningRequested()));
}
void ServerWidget::handleD(const QString &msg)
{
    if(m_Debug)
    {
        m_Debug->appendPlainText(msg);
    }
}
