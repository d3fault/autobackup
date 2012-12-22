#include "redparenthesisclosecontextrefresherwidget.h"

RedParenthesisCloseContextRefresherWidget::RedParenthesisCloseContextRefresherWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();
    m_Red = new RedParenthesisCloseContextRefresherPlainTextEdit();
    m_Debug = new QPlainTextEdit();

    m_Layout->addWidget(m_Red);
    m_Layout->addWidget(m_Debug);
    this->setLayout(m_Layout);

    connect(m_Red, SIGNAL(d(QString)), m_Debug, SLOT(appendPlainText(QString)));
}
RedParenthesisCloseContextRefresherWidget::~RedParenthesisCloseContextRefresherWidget()
{
    
}
