#include "debugserverview.h"

debugServerView::debugServerView(QWidget *parent)
    : QWidget(parent)
{
    m_BankThread = new QThread();
    m_Bank = new Bank();
    m_Bank->moveToThread(m_BankThread);
    m_BankThread->start();

    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_ListenButton = new QPushButton("Start");

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_ListenButton);
    this->setLayout(m_Layout);

    connect(m_ListenButton, SIGNAL(clicked()), m_Bank, SLOT(start()));
    connect(m_Bank, SIGNAL(d(QString)), m_Debug, SLOT(appendPlainText(QString)));
}
debugServerView::~debugServerView()
{

}
