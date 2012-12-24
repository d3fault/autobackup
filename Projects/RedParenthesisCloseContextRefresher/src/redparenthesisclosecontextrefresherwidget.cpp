#include "redparenthesisclosecontextrefresherwidget.h"

RedParenthesisCloseContextRefresherWidget::RedParenthesisCloseContextRefresherWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();
    QHBoxLayout *controlLayout = new QHBoxLayout();
    m_LeftButton = new QPushButton("<-");
    m_RightButton = new QPushButton("->");
    m_Red = new RedParenthesisCloseContextRefresherPlainTextEdit();
    m_Debug = new QPlainTextEdit();

    controlLayout->addWidget(m_LeftButton);
    controlLayout->addWidget(m_RightButton);
    m_Layout->addLayout(controlLayout);
    m_Layout->addWidget(m_Red);
    m_Layout->addWidget(m_Debug);
    this->setLayout(m_Layout);

    connect(m_Red, SIGNAL(d(QString)), m_Debug, SLOT(appendPlainText(QString)));
    connect(m_LeftButton, SIGNAL(clicked()), m_Red, SLOT(moveCursorLeft()), Qt::QueuedConnection);
    connect(m_RightButton, SIGNAL(clicked()), m_Red, SLOT(moveCursorRight()), Qt::QueuedConnection);
}
RedParenthesisCloseContextRefresherWidget::~RedParenthesisCloseContextRefresherWidget()
{
    
}
