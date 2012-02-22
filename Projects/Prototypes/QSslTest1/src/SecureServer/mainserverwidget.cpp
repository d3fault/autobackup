#include "mainserverwidget.h"

mainServerWidget::mainServerWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_StartButton = new QPushButton("Start");

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_StartButton);
    this->setLayout(m_Layout);

    m_WorkerThread = new QThread();
    m_SecureServer = new SecureServer();
    m_SecureServer->moveToThread(m_WorkerThread);
    m_WorkerThread->start();

    connect(m_StartButton, SIGNAL(clicked()), m_SecureServer, SLOT(startServer()));
    connect(m_SecureServer, SIGNAL(d(QString)), m_Debug, SLOT(appendPlainText(QString)));
}

mainServerWidget::~mainServerWidget()
{

}
