#include "mainclientwidget.h"

mainClientWidget::mainClientWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_StartStopButton = new QPushButton("Connect");

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_StartStopButton);
    this->setLayout(m_Layout);

    m_ClientThread = new QThread();
    m_Client = new ProtocolClient();
    m_Client->moveToThread(m_ClientThread);
    m_ClientThread->start();

    connect(m_StartStopButton, SIGNAL(clicked()), m_Client, SLOT(connectToServer()));
    connect(m_Client, SIGNAL(d(QString)), m_Debug, SLOT(appendPlainText(QString)));
}

mainClientWidget::~mainClientWidget()
{

}
