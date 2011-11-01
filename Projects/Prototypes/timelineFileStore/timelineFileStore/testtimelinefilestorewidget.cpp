#include "testtimelinefilestorewidget.h"

testTimelineFileStoreWidget::testTimelineFileStoreWidget(QWidget *parent)
    : QWidget(parent)
{
    //gui
    m_Layout = new QVBoxLayout();
    QString curDateTime(QDateTime::currentDateTime().toString());
    m_NewItemTitle = new QPlainTextEdit(QString("AutoTitle @") + curDateTime);
    m_NewItemContent = new QPlainTextEdit(QString("AutoBody @") + curDateTime);
    m_SubmitButton = new QPushButton("Submit/Save");
    m_Layout->addWidget(m_NewItemTitle);
    m_Layout->addWidget(m_NewItemContent);
    m_Layout->addWidget(m_SubmitButton);
    this->setLayout(m_Layout);

    //backend
    m_Timeline = new TimelineWithFileStore();

    //connections
    connect(m_SubmitButton, SIGNAL(clicked()), this, SLOT(handleSubmitButtonClicked()));
}
testTimelineFileStoreWidget::~testTimelineFileStoreWidget()
{

}
void testTimelineFileStoreWidget::handleSubmitButtonClicked()
{
    m_Timeline->addNode(new TimelineNode(TimelineNode::Text, m_NewItemTitle->document()->toPlainText(), m_NewItemContent->document()->toPlainText()));
}
