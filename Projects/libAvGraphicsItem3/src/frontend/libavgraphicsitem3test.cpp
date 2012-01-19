#include "libavgraphicsitem3test.h"

libAvGraphicsItem3test::libAvGraphicsItem3test(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();
    m_VideoPlayerWidget = new videoPlayerWidget();
    m_DebugOutput = new QPlainTextEdit();

    m_Layout->addWidget(m_VideoPlayerWidget);
    m_Layout->addWidget(m_DebugOutput);
    this->setLayout(m_Layout);
}
libAvGraphicsItem3test::~libAvGraphicsItem3test()
{

}
