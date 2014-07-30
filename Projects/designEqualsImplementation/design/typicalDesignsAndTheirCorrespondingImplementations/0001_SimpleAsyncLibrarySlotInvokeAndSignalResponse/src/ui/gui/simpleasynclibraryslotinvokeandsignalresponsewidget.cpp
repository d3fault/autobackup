#include "simpleasynclibraryslotinvokeandsignalresponsewidget.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>

SimpleAsyncLibrarySlotInvokeAndSignalResponseWidget::SimpleAsyncLibrarySlotInvokeAndSignalResponseWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    m_LineEdit = new QLineEdit();
    QPushButton *pushButton = new QPushButton("Foo Slot");
    m_Output = new QPlainTextEdit();

    myLayout->addWidget(m_LineEdit);
    myLayout->addWidget(pushButton);
    myLayout->addWidget(m_Output);

    setLayout(myLayout);

    connect(pushButton, SIGNAL(clicked()), this, SLOT(handleFooSlotButtonPressed()));
}
SimpleAsyncLibrarySlotInvokeAndSignalResponseWidget::~SimpleAsyncLibrarySlotInvokeAndSignalResponseWidget()
{ }
void SimpleAsyncLibrarySlotInvokeAndSignalResponseWidget::handleFooSlotButtonPressed()
{
    emit fooSlotRequested(m_LineEdit->text());
}
void SimpleAsyncLibrarySlotInvokeAndSignalResponseWidget::handleFooSignal(bool success)
{
    m_Output->appendPlainText("Foo Signal Success: " + QString((success ? "yes" : "no")));
}
