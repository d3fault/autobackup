#include "mainserverwidget.h"

mainServerWidget::mainServerWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_StartStopButton = new QPushButton("Start");

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_StartStopButton);
    this->setLayout(m_Layout);

    m_ServerThread = new QThread();
    m_Server = new ProtocolServer();
    m_Server->moveToThread(m_ServerThread);
    m_ServerThread->start();

    connect(m_StartStopButton, SIGNAL(clicked()), m_Server, SLOT(start()));
    connect(m_Server, SIGNAL(d(QString)), m_Debug, SLOT(appendPlainText(QString)));
}

mainServerWidget::~mainServerWidget()
{

}
