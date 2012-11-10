#include "serverwidget.h"

ServerWidget::ServerWidget(QWidget *parent)
    : QWidget(parent)
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

    this->setLayout(m_Layout);

    connect(m_StartAll3Listening, SIGNAL(clicked()), this, SIGNAL(startAll3ListeningRequested()));
}
